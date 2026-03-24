#include "JsonWriter.h"
#include <iostream>

namespace Metroidvania {

    JsonWriter::JsonWriter()
    {
        m_stream << "{\n";
        m_indent = 1;
    }

    // writes the finished JSON to a file
    bool JsonWriter::writeToFile(const std::string& path) const
    {
        std::ofstream file(path);
        if (!file.is_open())
        {
            std::cerr << "[JsonWriter] Could not open file: " << path << "\n";
            return false;
        }
        file << toString();
        return true;
    }

    // returns the JSON as a string
    std::string JsonWriter::toString() const
    {
        return m_stream.str() + "\n}";
    }

    // opens a named or unnamed object block
    void JsonWriter::beginObject(const std::string& key)
    {
        writeCommaIfNeeded();
        writeIndent();
        if (!key.empty())
            m_stream << "\"" << key << "\": {\n";
        else
            m_stream << "{\n";
        m_indent++;
        m_needsComma = false;
    }

    // closes an object block
    void JsonWriter::endObject()
    {
        m_indent--;
        m_stream << "\n";
        writeIndent();
        m_stream << "}";
        m_needsComma = true;
    }

    // opens a named or unnamed array block
    void JsonWriter::beginArray(const std::string& key)
    {
        writeCommaIfNeeded();
        writeIndent();
        if (!key.empty())
            m_stream << "\"" << key << "\": [\n";
        else
            m_stream << "[\n";
        m_indent++;
        m_needsComma = false;
    }

    // closes an array block
    void JsonWriter::endArray()
    {
        m_indent--;
        m_stream << "\n";
        writeIndent();
        m_stream << "]";
        m_needsComma = true;
    }

    // opens an object element inside an array
    void JsonWriter::beginArrayElement()
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "{\n";
        m_indent++;
        m_needsComma = false;
    }

    // closes an object element inside an array
    void JsonWriter::endArrayElement()
    {
        m_indent--;
        m_stream << "\n";
        writeIndent();
        m_stream << "}";
        m_needsComma = true;
    }

    void JsonWriter::writeString(const std::string& key, const std::string& value)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "\"" << key << "\": \"" << value << "\"";
        m_needsComma = true;
    }

    void JsonWriter::writeInt(const std::string& key, int value)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "\"" << key << "\": " << value;
        m_needsComma = true;
    }

    void JsonWriter::writeFloat(const std::string& key, float value)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "\"" << key << "\": " << value;
        m_needsComma = true;
    }

    void JsonWriter::writeBool(const std::string& key, bool value)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "\"" << key << "\": " << (value ? "true" : "false");
        m_needsComma = true;
    }

    // writes a compact inline int array - used for tile entries [col, row, type]
    void JsonWriter::writeIntArray(const std::string& key, const std::vector<int>& values)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "\"" << key << "\": [";
        for (int i = 0; i < (int)values.size(); ++i)
        {
            if (i > 0) m_stream << ", ";
            m_stream << values[i];
        }
        m_stream << "]";
        m_needsComma = true;
    }

    void JsonWriter::writeRawIntArray(const std::vector<int>& values)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "[";
        for (int i = 0; i < (int)values.size(); ++i)
        {
            if (i > 0) m_stream << ", ";
            m_stream << values[i];
        }
        m_stream << "]";
        m_needsComma = true;
    }

    // writes a compact inline float array - used for color and position entries
    void JsonWriter::writeFloatArray(const std::string& key, const std::vector<float>& values)
    {
        writeCommaIfNeeded();
        writeIndent();
        m_stream << "\"" << key << "\": [";
        for (int i = 0; i < (int)values.size(); ++i)
        {
            if (i > 0) m_stream << ", ";
            m_stream << values[i];
        }
        m_stream << "]";
        m_needsComma = true;
    }

    // writes a comma before the next entry if one is needed
    void JsonWriter::writeCommaIfNeeded()
    {
        if (m_needsComma)
            m_stream << ",\n";
        m_needsComma = false;
    }

    // writes the current indent level as spaces
    void JsonWriter::writeIndent()
    {
        for (int i = 0; i < m_indent; ++i)
            m_stream << "    ";
    }

    // writes a quoted key followed by a colon
    void JsonWriter::writeKey(const std::string& key)
    {
        m_stream << "\"" << key << "\": ";
    }

    // returns the current indent as a string
    std::string JsonWriter::indentString() const
    {
        std::string s;
        for (int i = 0; i < m_indent; ++i)
            s += "    ";
        return s;
    }

}