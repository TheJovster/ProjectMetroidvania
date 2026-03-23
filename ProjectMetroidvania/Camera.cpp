#include "Camera.h"
#include <algorithm>
#include <cmath>

namespace Metroidvania {

    Camera::Camera(sf::Vector2u windowSize)
    {
        m_view.setSize(sf::Vector2f(windowSize));
        m_view.setCenter(sf::Vector2f(
            windowSize.x * 0.5f,
            windowSize.y * 0.5f
        ));

        m_target = m_view.getCenter();
        m_roomBounds = sf::FloatRect({ 0.f, 0.f },
            sf::Vector2f(windowSize));
    }

    void Camera::update(float dt, sf::Vector2f playerPosition, bool playerFacingRight)
    {
        switch (m_mode)
        {
        case CameraMode::Free:
            updateFree(dt, playerPosition, playerFacingRight);
            break;

        case CameraMode::Locked:
            // Nothing - camera stays where it was set
            break;

        case CameraMode::Transition:
            updateTransition(dt);
            break;
        }
    }

    
    void Camera::apply(sf::RenderWindow& window) const
    {
        window.setView(m_view);
    }

    //Room Settings
    void Camera::setRoomBounds(sf::FloatRect bounds)
    {
        m_roomBounds = bounds;
    }

    void Camera::setMode(CameraMode mode)
    {
        m_mode = mode;
    }

    void Camera::snapTo(sf::Vector2f position)
    {
        sf::Vector2f clamped = clampPosition(position, m_roomBounds);
        m_view.setCenter(clamped);
        m_target = clamped;
    }

    //Transition
    void Camera::beginTransition(sf::FloatRect newRoomBounds)
    {
        m_nextRoomBounds = newRoomBounds;
        m_mode = CameraMode::Transition;
        m_transitioning = true;
    }

    //Queries
    sf::Vector2f Camera::getPosition() const
    {
        return m_view.getCenter();
    }

    sf::FloatRect Camera::getViewBounds() const
    {
        const sf::Vector2f halfSize = m_view.getSize() / 2.f;
        const sf::Vector2f centre = m_view.getCenter();

        return sf::FloatRect(
            centre - halfSize,
            m_view.getSize()
        );
    }

    //Internal - Free Follow
    void Camera::updateFree(float dt,
        sf::Vector2f playerPosition,
        bool playerFacingRight)
    {
        // Centre on player + lookahead offset in facing direction
        const float lookAheadDir = playerFacingRight ? 1.f : -1.f;

        m_target = sf::Vector2f(
            playerPosition.x + (k_cameraLookAhead * lookAheadDir),
            playerPosition.y
        );

        // Lerp current centre toward target
        sf::Vector2f current = m_view.getCenter();
        sf::Vector2f next = current + (m_target - current)
            * (k_cameraLerpSpeed * dt);

        // Clamp to room bounds before applying
        m_view.setCenter(clampPosition(next, m_roomBounds));
    }

    //Internal - Transition/Transition Slide
    void Camera::updateTransition(float dt)
    {
        // Target is the centre of the next room
        const sf::Vector2f nextCentre = sf::Vector2f(
            m_nextRoomBounds.position.x + m_nextRoomBounds.size.x * 0.5f,
            m_nextRoomBounds.position.y + m_nextRoomBounds.size.y * 0.5f
        );

        sf::Vector2f current = m_view.getCenter();
        sf::Vector2f delta = nextCentre - current;
        float        distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        const float step = k_transitionSpeed * dt;

        if (distance <= step)
        {
            // Arrived - snap to centre, swap to new room, return to Free
            m_view.setCenter(nextCentre);
            m_roomBounds = m_nextRoomBounds;
            m_mode = CameraMode::Free;
            m_transitioning = false;
        }
        else
        {
            // Move toward target at fixed speed - no lerp during transition
            sf::Vector2f direction = delta / distance;
            m_view.setCenter(current + direction * step);
        }
    }

    //Internal - Clamp
    void Camera::clampToBounds()
    {
        m_view.setCenter(clampPosition(m_view.getCenter(), m_roomBounds));
    }

    sf::Vector2f Camera::clampPosition(sf::Vector2f position,
        sf::FloatRect bounds) const
    {
        const sf::Vector2f halfSize = m_view.getSize() / 2.f;

        // Minimum and maximum centres that keep the view inside bounds
        const float minX = bounds.position.x + halfSize.x;
        const float maxX = bounds.position.x + bounds.size.x - halfSize.x;
        const float minY = bounds.position.y + halfSize.y;
        const float maxY = bounds.position.y + bounds.size.y - halfSize.y;

        // If the room is smaller than the view on an axis,
        // lock to the room centre on that axis rather than clamping
        float x = (maxX > minX)
            ? std::clamp(position.x, minX, maxX)
            : bounds.position.x + bounds.size.x * 0.5f;

        float y = (maxY > minY)
            ? std::clamp(position.y, minY, maxY)
            : bounds.position.y + bounds.size.y * 0.5f;

        return sf::Vector2f(x, y);
    }

}