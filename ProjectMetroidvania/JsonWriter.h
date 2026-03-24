#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace Metroidvania {

    class JsonWriter {
    public:
        JsonWriter();

        //Output
        bool writeToFile(const std::string& path) const;
        std::string toString() const;

        //Object / array scope
        void beginObject(const std::string& key = "");
        void endObject();
        void beginArray(const std::string& key = "");
        void endArray();

        //Value writers
        void writeString(const std::string& key, const std::string& value);
        void writeInt(const std::string& key, int value);
        void writeFloat(const std::string& key, float value);
        void writeBool(const std::string& key, bool value);

        //Inline array helpers - for compact rows like [col, row, tileType]
        void writeIntArray(const std::string& key, const std::vector<int>& values);
        void writeRawIntArray(const std::vector<int>& values);
        void writeFloatArray(const std::string& key, const std::vector<float>& values);

        //Raw array element - for writing objects inside arrays
        void beginArrayElement();
        void endArrayElement();

    private:
        std::ostringstream m_stream;
        int                m_indent = 0;
        bool               m_needsComma = false;

        //Helpers
        void        writeIndent();
        void        writeCommaIfNeeded();
        void        writeKey(const std::string& key);
        std::string indentString() const;
    };

}