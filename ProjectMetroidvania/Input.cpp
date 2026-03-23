#include "Input.h"

namespace Metroidvania {

    void Input::update(const sf::Event& event)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            auto action = mapKeyToAction(keyPressed->code);
            if (action.has_value())
            {
                const int idx = static_cast<int>(action.value());

                if (!m_held[idx])
                {
                    m_pressed[idx] = true;

                    // Push to buffer, cap at k_inputBufferDepth
                    if ((int)m_buffer.size() >= k_inputBufferDepth)
                        m_buffer.pop_front();

                    m_buffer.push_back({ action.value(), 0 });
                }

                m_held[idx] = true;
            }
        }

        if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
        {
            auto action = mapKeyToAction(keyReleased->code);
            if (action.has_value())
            {
                const int idx = static_cast<int>(action.value());
                m_held[idx] = false;
                m_released[idx] = true;
            }
        }

        // --- Mouse position ---
        if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
        {
            m_mousePosition = sf::Vector2i(mouseMoved->position.x, mouseMoved->position.y);
        }

        // --- Mouse buttons ---
        if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            const int idx = static_cast<int>(mousePressed->button);
            if (idx < 3)
            {
                m_mouseHeld[idx] = true;
                m_mousePressed[idx] = true;
            }
        }

        if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
        {
            const int idx = static_cast<int>(mouseReleased->button);
            if (idx < 3)
            {
                m_mouseHeld[idx] = false;
                m_mouseReleased[idx] = true;
            }
        }
    }

    void Input::tick()
    {
        // Clear per-frame state
        m_pressed.fill(false);
        m_released.fill(false);
        m_mousePressed.fill(false);
        m_mouseReleased.fill(false);

        // Age the buffer - remove entries older than k_inputBufferDepth
        for (auto& entry : m_buffer)
            entry.framesAgo++;

        while (!m_buffer.empty() && m_buffer.front().framesAgo >= k_inputBufferDepth)
            m_buffer.pop_front();
    }

    bool Input::isHeld(Action action) const
    {
        return m_held[static_cast<int>(action)];
    }

    bool Input::isJustPressed(Action action) const
    {
        return m_pressed[static_cast<int>(action)];
    }

    bool Input::isJustReleased(Action action) const
    {
        return m_released[static_cast<int>(action)];
    }

    bool Input::wasRecentlyPressed(Action action) const
    {
        for (const auto& entry : m_buffer)
            if (entry.action == action)
                return true;
        return false;
    }

    void Input::consumeBufferedAction(Action action)
    {
        for (auto it = m_buffer.begin(); it != m_buffer.end(); ++it)
        {
            if (it->action == action)
            {
                m_buffer.erase(it);
                return;
            }
        }
    }

    bool Input::isMouseButtonHeld(sf::Mouse::Button button) const
    {
        return m_mouseHeld[static_cast<int>(button)];
    }

    bool Input::isMouseButtonPressed(sf::Mouse::Button button) const
    {
        return m_mousePressed[static_cast<int>(button)];
    }

    bool Input::isMouseButtonReleased(sf::Mouse::Button button) const
    {
        return m_mouseReleased[static_cast<int>(button)];
    }

    std::optional<Action> Input::mapKeyToAction(sf::Keyboard::Key key) const
    {
        switch (key)
        {
            // --- Movement ---
        case sf::Keyboard::Key::A:      return Action::MoveLeft;
        case sf::Keyboard::Key::D:      return Action::MoveRight;
        case sf::Keyboard::Key::S:      return Action::Crouch;
        case sf::Keyboard::Key::Space:  return Action::Jump;

            // --- Combat ---
        case sf::Keyboard::Key::J:      return Action::AttackPrimary;
        case sf::Keyboard::Key::K:      return Action::AttackSecondary;

            // --- Abilities ---
        case sf::Keyboard::Key::U:      return Action::AbilityPrimary;
        case sf::Keyboard::Key::I:      return Action::AbilitySecondary;

            // --- Interaction ---
        case sf::Keyboard::Key::E:      return Action::Interact;

            // --- Meta ---
        case sf::Keyboard::Key::Escape: return Action::Pause;

        default: return std::nullopt;
        }
    }
}