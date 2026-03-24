#pragma once
#include <SFML/Graphics.hpp>
#include "TileMap.h"

namespace Metroidvania {

    //Scroll speeds relative to camera movement
    constexpr float k_parallaxMidSpeed = 0.1f;  // 10% of camera speed - very slow drift
    constexpr float k_parallaxFrontSpeed = 0.4f;  // 40% of camera speed - noticeable scroll

    //Tint colors - darken layers furthest from player
    constexpr sf::Color k_parallaxBgTint = sf::Color(160, 160, 160, 255); // darkest
    constexpr sf::Color k_parallaxMidTint = sf::Color(200, 200, 200, 255); // mid dark
    constexpr sf::Color k_parallaxFrontTint = sf::Color(230, 230, 230, 255); // closest to normal

    class ParallaxLayer {
    public:
        ParallaxLayer(
            sf::Vector2u  windowSize,
            const std::string& plxBkgPath,   // path to background texture - empty = solid color
            const std::string& plxMidPath,   // path to mid texture - empty = solid color
            sf::Color          bgColor = sf::Color(20, 20, 40) // default dark blue-black
        );

        //Update - call with current camera position every frame
        void update(sf::Vector2f cameraPosition);

        //Draw - split into world and overlay passes
        void draw(sf::RenderWindow& window) const;

        //Parallax front TileMap - exposed for DevMode
        TileMap& getFrontLayer() { return m_frontTileMap; }

    private:
        sf::Vector2u m_windowSize;

        //Background - static, screen sized, always centered on camera
        sf::RectangleShape m_bgShape;       // solid color fallback
        sf::Texture        m_bgTexture;
        std::optional<sf::Sprite> m_bgSprite;
        bool               m_bgHasTexture = false;

        //Mid layer - scrolling sprite
        sf::Texture        m_midTexture;
        std::optional<sf::Sprite> m_midSprite;
        bool               m_midHasTexture = false;
        float              m_midOffset = 0.f;   // current horizontal scroll offset

        //Front layer - scrolling TileMap
        TileMap            m_frontTileMap;
        float              m_frontOffset = 0.f;   // current horizontal scroll offset

        //Last camera position for delta calculation
        sf::Vector2f       m_lastCameraPos;
        bool               m_firstUpdate = true;

        //Helpers
        void drawScrollingSprite(sf::RenderWindow& window,
            sf::Sprite sprite,
            float             offset,
            sf::Color         tint) const;

        float wrapOffset(float offset, float width) const;
    };

}