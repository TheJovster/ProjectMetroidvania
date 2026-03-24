#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Metroidvania {

    struct JsonValue {

        enum class Type {
            Null,
            Bool,
            Int,
            Float,
            String,
            Array,
            Object
        };

        Type type = Type::Null;

        bool        boolVal = false;
        int         intVal = 0;
        float       floatVal = 0.f;
        std::string stringVal = "";

        std::vector<JsonValue>                      arrayVal;
        std::unordered_map<std::string, JsonValue>  objectVal;

        bool isNull()   const { return type == Type::Null; }
        bool isBool()   const { return type == Type::Bool; }
        bool isInt()    const { return type == Type::Int; }
        bool isFloat()  const { return type == Type::Float; }
        bool isString() const { return type == Type::String; }
        bool isArray()  const { return type == Type::Array; }
        bool isObject() const { return type == Type::Object; }

        // returns default if wrong type
        bool        getBool(bool              def = false) const { return isBool() ? boolVal : def; }
        int         getInt(int               def = 0)     const { return isInt() ? intVal : def; }
        float       getFloat(float             def = 0.f)   const { return isFloat() ? floatVal : def; }
        std::string getString(const std::string& def = "")   const { return isString() ? stringVal : def; }

        // returns null JsonValue if key missing
        bool             hasKey(const std::string& key) const;
        const JsonValue& operator[](const std::string& key) const;
        const JsonValue& operator[](int index) const;

        int size() const { return static_cast<int>(arrayVal.size()); }
    };

    class JsonReader {
    public:
        // reads and parses a JSON file - returns false on failure
        bool loadFromFile(const std::string& path);

        const JsonValue& getRoot()  const { return m_root; }
        const std::string& getError() const { return m_error; }
        bool               hasError() const { return !m_error.empty(); }

    private:
        JsonValue   m_root;
        std::string m_source;  // raw JSON text
        std::string m_error;   // last error message
        int         m_pos = 0; // current parse position

        JsonValue parseValue();
        JsonValue parseObject();
        JsonValue parseArray();
        JsonValue parseString();
        JsonValue parseNumber();
        JsonValue parseLiteral(); // true, false, null

        void        skipWhitespace();
        char        peek()  const;
        char        consume();
        bool        expect(char c);
        bool        isEnd() const;
        void        setError(const std::string& msg);
        std::string readString(); // reads a quoted string value
    };

}