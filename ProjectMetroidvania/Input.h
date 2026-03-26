#pragma once
#include <array>
#include <deque>
#include <optional>
#include <SFML/Window.hpp>
#include "Action.h"

namespace Metroidvania {

    // Input buffer depth - start at 10 frames, tune as needed
    constexpr int k_inputBufferDepth = 10;

    struct BufferedAction {
        Action      action;
        int         framesAgo;  // how many frames ago this was pressed
    };

    class Input {
    public:
        // Called once per frame from Game, consumes sf::Event
        void update(const sf::Event& event);

        // Called at the END of each frame to age the buffer and reset per-frame state
        void tick();

        // Stateful queries
        bool isHeld(Action action) const;
        bool isJustPressed(Action action) const;
        bool isJustReleased(Action action) const;

        // Buffer queries
        // Returns true if action was pressed within the last k_inputBufferDepth frames
        bool wasRecentlyPressed(Action action) const;

        // Consume a buffered action so it isn't re-read next frame
        void consumeBufferedAction(Action action);

        // Mouse
        sf::Vector2i mousePosition() const { return m_mousePosition; }
        bool isMouseButtonHeld(sf::Mouse::Button button) const;
        bool isMouseButtonPressed(sf::Mouse::Button button) const;
        bool isMouseButtonReleased(sf::Mouse::Button button) const;

    private:
        // Action state arrays - indexed by Action enum
        std::array<bool, static_cast<int>(Action::COUNT)> m_held = {};
        std::array<bool, static_cast<int>(Action::COUNT)> m_pressed = {};
        std::array<bool, static_cast<int>(Action::COUNT)> m_released = {};

        // Input buffer - bounded deque of recent presses
        std::deque<BufferedAction> m_buffer;

        // Mouse
        sf::Vector2i m_mousePosition;
        std::array<bool, 3> m_mouseHeld = {};
        std::array<bool, 3> m_mousePressed = {};
        std::array<bool, 3> m_mouseReleased = {};

        // Maps SFML key to Action - returns nullopt if key isn't bound
        std::optional<Action> mapKeyToAction(sf::Keyboard::Key key) const;
    };

}