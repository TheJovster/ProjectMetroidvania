#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include "TileMap.h"
#include "Camera.h"

namespace Metroidvania {

    enum class EditorLayer {
        Background = 0,
        Midground = 1,
        Foreground = 2,
        COUNT
    };

    constexpr std::array<TileType, 3> k_editableTiles = {
        TileType::Stone,
        TileType::Platform,
        TileType::Empty
    };

    constexpr float k_cameraEditorSpeed = 600.f;  // pixels per second - tune as needed

    class DevMode {
    public:
        DevMode(TileMap& bg, TileMap& mg, TileMap& fg, Camera& camera);

        // --- Toggle ---
        void setActive(bool active);
        bool isActive() const { return m_active; }

        // --- Update ---
        void update(float dt, const sf::RenderWindow& window);

        // --- Input --- 
        void handleEvent(const sf::Event& event);

        // --- Render ---
        void drawWorld(sf::RenderWindow& window) const;  // ghost tile - in world space
        void drawHUD(sf::RenderWindow& window) const;  // layer/tile info - in screen space

    private:
        std::array<TileMap*, static_cast<int>(EditorLayer::COUNT)> m_layers;
        Camera& m_camera;

        bool       m_active = false;
        int        m_layerIndex = 1;        // default to midground
        int        m_tileIndex = 0;        // index into k_editableTiles
        bool       m_painting = false;    // left mouse held
        bool       m_erasing = false;    // right mouse held

        sf::RectangleShape m_ghost;

        sf::Font   m_font;
        bool       m_fontLoaded = false;

        sf::Vector2f m_camVelocity;

        TileMap* activeLayer()     const { return m_layers[m_layerIndex]; }
        TileType         activeTileType()  const { return k_editableTiles[m_tileIndex]; }
        sf::Vector2f     mouseWorldPos(const sf::RenderWindow& window) const;
        std::string      layerName()       const;
        std::string      tileName()        const;

        void             paintAt(sf::Vector2f worldPos);
        void             eraseAt(sf::Vector2f worldPos);
        void             cycleLayer(int dir);
        void             cycleTile(int dir);
        void             updateGhost(sf::Vector2f worldPos);
        void             updateCameraMovement(float dt);
    };

}