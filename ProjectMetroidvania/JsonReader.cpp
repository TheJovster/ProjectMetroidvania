#include "JsonReader.h"
#include <fstream>
#include <sstream>

namespace Metroidvania {

    static const JsonValue k_nullValue;

    bool JsonValue::hasKey(const std::string& key) const
    {
        if (!isObject()) return false;
        return objectVal.find(key) != objectVal.end();
    }

    const JsonValue& JsonValue::operator[](const std::string& key) const
    {
        if (!isObject()) return k_nullValue;
        auto it = objectVal.find(key);
        if (it == objectVal.end()) return k_nullValue;
        return it->second;
    }

    const JsonValue& JsonValue::operator[](int index) const
    {
        if (!isArray()) return k_nullValue;
        if (index < 0 || index >= (int)arrayVal.size()) return k_nullValue;
        return arrayVal[index];
    }

    bool JsonReader::loadFromFile(const std::string& path)
    {
        // read file into string
        std::ifstream file(path);
        if (!file.is_open())
        {
            setError("Could not open file: " + path);
            return false;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        m_source = ss.str();
        m_pos = 0;
        m_error = "";

        skipWhitespace();
        m_root = parseValue();

        return !hasError();
    }

    JsonValue JsonReader::parseValue()
    {
        skipWhitespace();

        if (isEnd())
        {
            setError("Unexpected end of input");
            return {};
        }

        char c = peek();

        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return parseString();
        if (c == 't' || c == 'f' || c == 'n') return parseLiteral();
        if (c == '-' || std::isdigit(c)) return parseNumber();

        setError(std::string("Unexpected character: ") + c);
        return {};
    }

    JsonValue JsonReader::parseObject()
    {
        JsonValue val;
        val.type = JsonValue::Type::Object;

        consume(); // opening {
        skipWhitespace();

        if (peek() == '}')
        {
            consume(); // empty object
            return val;
        }

        while (!isEnd())
        {
            skipWhitespace();

            // read key
            if (peek() != '"')
            {
                setError("Expected string key in object");
                return {};
            }

            std::string key = readString();
            if (hasError()) return {};

            skipWhitespace();

            if (!expect(':'))
            {
                setError("Expected ':' after key: " + key);
                return {};
            }

            skipWhitespace();

            // read value
            JsonValue child = parseValue();
            if (hasError()) return {};

            val.objectVal[key] = std::move(child);

            skipWhitespace();

            if (peek() == '}')
            {
                consume();
                return val;
            }

            if (!expect(','))
            {
                setError("Expected ',' or '}' in object");
                return {};
            }
        }

        setError("Unterminated object");
        return {};
    }

    JsonValue JsonReader::parseArray()
    {
        JsonValue val;
        val.type = JsonValue::Type::Array;

        consume(); // opening [
        skipWhitespace();

        if (peek() == ']')
        {
            consume(); // empty array
            return val;
        }

        while (!isEnd())
        {
            skipWhitespace();

            JsonValue element = parseValue();
            if (hasError()) return {};

            val.arrayVal.push_back(std::move(element));

            skipWhitespace();

            if (peek() == ']')
            {
                consume();
                return val;
            }

            if (!expect(','))
            {
                setError("Expected ',' or ']' in array");
                return {};
            }
        }

        setError("Unterminated array");
        return {};
    }

    JsonValue JsonReader::parseString()
    {
        JsonValue val;
        val.type = JsonValue::Type::String;
        val.stringVal = readString();
        return val;
    }

    JsonValue JsonReader::parseNumber()
    {
        // collect the number characters
        std::string raw;
        bool isFloat = false;

        if (peek() == '-') raw += consume();

        while (!isEnd() && std::isdigit(peek()))
            raw += consume();

        if (!isEnd() && peek() == '.')
        {
            isFloat = true;
            raw += consume();
            while (!isEnd() && std::isdigit(peek()))
                raw += consume();
        }

        // handle exponent
        if (!isEnd() && (peek() == 'e' || peek() == 'E'))
        {
            isFloat = true;
            raw += consume();
            if (!isEnd() && (peek() == '+' || peek() == '-'))
                raw += consume();
            while (!isEnd() && std::isdigit(peek()))
                raw += consume();
        }

        JsonValue val;
        if (isFloat)
        {
            val.type = JsonValue::Type::Float;
            val.floatVal = std::stof(raw);
        }
        else
        {
            val.type = JsonValue::Type::Int;
            val.intVal = std::stoi(raw);
        }

        return val;
    }

    JsonValue JsonReader::parseLiteral()
    {
        JsonValue val;

        if (m_source.substr(m_pos, 4) == "true")
        {
            val.type = JsonValue::Type::Bool;
            val.boolVal = true;
            m_pos += 4;
        }
        else if (m_source.substr(m_pos, 5) == "false")
        {
            val.type = JsonValue::Type::Bool;
            val.boolVal = false;
            m_pos += 5;
        }
        else if (m_source.substr(m_pos, 4) == "null")
        {
            val.type = JsonValue::Type::Null;
            m_pos += 4;
        }
        else
        {
            setError(std::string("Unknown literal at position ") + std::to_string(m_pos));
        }

        return val;
    }

    std::string JsonReader::readString()
    {
        if (!expect('"'))
        {
            setError("Expected opening quote");
            return {};
        }

        std::string result;

        while (!isEnd())
        {
            char c = consume();

            if (c == '"') return result; // closing quote

            // handle escape sequences
            if (c == '\\')
            {
                if (isEnd())
                {
                    setError("Unexpected end in escape sequence");
                    return {};
                }

                char esc = consume();
                switch (esc)
                {
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case '/':  result += '/';  break;
                case 'n':  result += '\n'; break;
                case 't':  result += '\t'; break;
                case 'r':  result += '\r'; break;
                default:
                    setError(std::string("Unknown escape: \\") + esc);
                    return {};
                }
            }
            else
            {
                result += c;
            }
        }

        setError("Unterminated string");
        return {};
    }

    void JsonReader::skipWhitespace()
    {
        while (!isEnd() && std::isspace(m_source[m_pos]))
            m_pos++;
    }

    char JsonReader::peek() const
    {
        if (isEnd()) return '\0';
        return m_source[m_pos];
    }

    char JsonReader::consume()
    {
        if (isEnd()) return '\0';
        return m_source[m_pos++];
    }

    bool JsonReader::expect(char c)
    {
        if (peek() != c) return false;
        consume();
        return true;
    }

    bool JsonReader::isEnd() const
    {
        return m_pos >= (int)m_source.size();
    }

    void JsonReader::setError(const std::string& msg)
    {
        m_error = msg;
    }

}