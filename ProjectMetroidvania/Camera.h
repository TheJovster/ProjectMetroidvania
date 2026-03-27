#pragma once
#include <SFML/Graphics.hpp>

namespace Metroidvania {

    enum class CameraMode {
        Free,           // follows player, clamped to room bounds
        Locked,         // fixed position, no scrolling
        Transition      // sliding from one room to another
    };

    // --- Tuning constants ---
    constexpr float k_cameraLerpSpeed = 5.f;     // higher = snappier follow, tune during feel pass
    constexpr float k_cameraLookAhead = 80.f;    // pixels ahead of player in movement direction
    constexpr float k_transitionSpeed = 800.f;   // pixels per second during room transition
    constexpr float k_cameraZoom = 0.4f;

    class Camera {
    public:
        explicit Camera(sf::Vector2u windowSize);

        // Call once per frame before render
        void update(float dt, sf::Vector2f playerPosition, bool playerFacingRight);

        void apply(sf::RenderWindow& window) const;

        void setRoomBounds(sf::FloatRect bounds);
        void setMode(CameraMode mode);
        void snapTo(sf::Vector2f position);     //instant - no lerp

        // --- Transition ---
        // Initiates a slide to a new room
        void beginTransition(sf::FloatRect newRoomBounds);
        bool isTransitioning() const { return m_transitioning; }

        // --- Queries ---
        CameraMode       getMode()      const { return m_mode; }
        sf::FloatRect    getRoomBounds()const { return m_roomBounds; }
        sf::Vector2f     getPosition()  const;
        sf::FloatRect    getViewBounds()const;  // what the camera currently sees

        const sf::View& getView() const { return m_view; }

    private:
        sf::View         m_view;
        sf::Vector2f     m_target;          // where the camera wants to be
        sf::FloatRect    m_roomBounds;      // current room clamp bounds
        sf::FloatRect    m_nextRoomBounds;  // target room during transition
        CameraMode       m_mode = CameraMode::Free;
        bool             m_transitioning = false;

        void updateFree(float dt, sf::Vector2f playerPosition, bool playerFacingRight);
        void updateTransition(float dt);
        void clampToBounds();

        sf::Vector2f clampPosition(sf::Vector2f position, sf::FloatRect bounds) const;
    };

}