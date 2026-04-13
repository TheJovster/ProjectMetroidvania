#ifdef _DEBUG

#include "Debug.h"
#include <iostream>
#include <chrono>
#include <ctime>

namespace Metroidvania {
    namespace Debug {

        static std::ofstream s_logFile;
        static sf::Font      s_font;        
        static bool          s_fontReady = false;
        static bool          s_ready = false;

        static std::string categoryToString(DebugCategory category)
        {
            switch (category)
            {
            case DebugCategory::Physics:    return "PHYSICS";
            case DebugCategory::Combat:     return "COMBAT";
            case DebugCategory::Animation:  return "ANIMATION";
            case DebugCategory::Input:      return "INPUT";
            case DebugCategory::AI:         return "AI";
            case DebugCategory::Audio:      return "AUDIO";
            case DebugCategory::Serializer: return "SERIALIZER";
            case DebugCategory::HUD:        return "HUD";
            case DebugCategory::General:    return "GENERAL";
            default:                        return "UNKNOWN";
            }
        }

        static std::string timestamp()
        {
            const auto now = std::chrono::system_clock::now();
            const auto time = std::chrono::system_clock::to_time_t(now);

            char buf[16];
            std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&time));
            return std::string(buf);
        }

        static void write(const std::string& level,
            DebugCategory      category,
            const std::string& message)
        {
            const std::string line = "[" + timestamp() + "] "
                + "[" + level + "] "
                + "[" + categoryToString(category) + "] "
                + message;

            std::cout << line << "\n";

            if (s_logFile.is_open())
                s_logFile << line << "\n";
        }

        void init(const std::string& logPath)
        {
            s_logFile.open(logPath, std::ios::out | std::ios::trunc);
            if (!s_logFile.is_open())
                std::cerr << "[Debug] Failed to open log file: " << logPath << "\n";

            s_ready = true;
            log(DebugCategory::General, "Debug logger initialized");
        }

        void shutdown()
        {
            log(DebugCategory::General, "Debug logger shutting down");
            if (s_logFile.is_open())
                s_logFile.close();
            s_ready = false;
        }

        void log(DebugCategory category, const std::string& message)
        {
            write("LOG", category, message);
        }

        void warn(DebugCategory category, const std::string& message)
        {
            write("WARN", category, message);
        }

        void error(DebugCategory category, const std::string& message)
        {
            write("ERROR", category, message);
        }

        void setFont(const sf::Font& font)
        {
            s_font = font;
            s_fontReady = true;
        }

        void drawText(sf::RenderWindow& window,
            const std::string& message,
            sf::Vector2f       position,
            sf::Color          color)
        {
            if (!s_fontReady) return;

            sf::Text text(s_font, message, 16);
            text.setFillColor(color);
            text.setPosition(position);
            window.draw(text);
        }

    } 
} 

#endif