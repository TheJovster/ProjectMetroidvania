#pragma once

#ifdef _DEBUG

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

namespace Metroidvania {

    enum class DebugCategory {
        Physics,
        Combat,
        Animation,
        Input,
        AI,
        Audio,
        Serializer,
        HUD,
        General
    };

    namespace Debug {

        void init(const std::string& logPath = "debug.log");

        void shutdown();

        void log(DebugCategory category, const std::string& message);
        void warn(DebugCategory category, const std::string& message);
        void error(DebugCategory category, const std::string& message);

        void drawText(sf::RenderWindow& window,
            const std::string& message,
            sf::Vector2f position,
            sf::Color color = sf::Color::White);

        void setFont(const sf::Font& font);

    } 

} 

#else
namespace Metroidvania {

    enum class DebugCategory { Physics, Combat, Animation, Input, AI, Audio, Serializer, HUD, General };

    namespace Debug {
        inline void init(const std::string & = "debug.log") {}
        inline void shutdown() {}
        inline void log(DebugCategory, const std::string&) {}
        inline void warn(DebugCategory, const std::string&) {}
        inline void error(DebugCategory, const std::string&) {}
        inline void setFont(sf::Font&) {}
    }

}

#endif