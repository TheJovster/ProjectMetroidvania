#include "ParallaxLayer.h"
#include <cmath>

namespace Metroidvania {

    ParallaxLayer::ParallaxLayer(
        sf::Vector2u       windowSize,
        const std::string& plxBkgPath,
        const std::string& plxMidPath,
        sf::Color          bgColor)
        : m_windowSize(windowSize)
        , m_frontTileMap(
            static_cast<int>((windowSize.x * 2) / k_tileSize) + 1,
            static_cast<int>(windowSize.y / k_tileSize) + 1)
    {
        // Background shape - solid color fallback, always screen sized
        m_bgShape.setSize(sf::Vector2f(windowSize));
        m_bgShape.setFillColor(bgColor);

        // Try to load background texture
        if (!plxBkgPath.empty() && m_bgTexture.loadFromFile(plxBkgPath))
        {
            m_bgSprite.emplace(m_bgTexture);
            m_bgHasTexture = true;
        }

        // Try to load mid texture
        if (!plxMidPath.empty() && m_midTexture.loadFromFile(plxMidPath))
        {
            m_midSprite.emplace(m_midTexture);
            m_midHasTexture = true;
        }
    }

    void ParallaxLayer::update(sf::Vector2f cameraPosition)
    {
        if (m_firstUpdate)
        {
            m_lastCameraPos = cameraPosition;
            m_firstUpdate = false;
            return;
        }

        // Delta is how far the camera moved this frame
        const float cameraDelta = cameraPosition.x - m_lastCameraPos.x;
        m_lastCameraPos = cameraPosition;

        // Scroll mid and front by their respective fractions of camera movement
        m_midOffset += cameraDelta * k_parallaxMidSpeed;
        m_frontOffset += cameraDelta * k_parallaxFrontSpeed;

        // Wrap offsets so they loop seamlessly
        if (m_midHasTexture)
        {
            const float texWidth = static_cast<float>(m_midTexture.getSize().x);
            m_midOffset = wrapOffset(m_midOffset, texWidth);
        }

        const float frontWidth = m_frontTileMap.getWidth();
        m_frontOffset = wrapOffset(m_frontOffset, frontWidth);
    }

    void ParallaxLayer::draw(sf::RenderWindow& window) const
    {
        const sf::Vector2f camCentre = window.getView().getCenter();
        const sf::Vector2f halfSize = sf::Vector2f(m_windowSize) * 0.5f;

        // Background - always centered on camera, drawn first
        if (m_bgHasTexture)
        {
            sf::Sprite bgDraw = *m_bgSprite;
            bgDraw.setPosition(camCentre - halfSize);
            bgDraw.setColor(k_parallaxBgTint);
            window.draw(bgDraw);
        }
        else
        {
            sf::RectangleShape bgDraw = m_bgShape;
            bgDraw.setPosition(camCentre - halfSize);
            window.draw(bgDraw);
        }

        // Mid layer - scrolling sprite
        if (m_midHasTexture)
            drawScrollingSprite(window, *m_midSprite, m_midOffset, k_parallaxMidTint);

        // Front layer - scrolling TileMap drawn with offset
        // Draw twice side by side for seamless loop
        const sf::Vector2f frontOffsetVec(m_frontOffset, 0.f);
        const sf::Vector2f frontWidthVec(m_frontTileMap.getWidth(), 0.f);

        m_frontTileMap.draw(window, frontOffsetVec);
        m_frontTileMap.draw(window, frontOffsetVec - frontWidthVec);
    }

    void ParallaxLayer::drawScrollingSprite(
        sf::RenderWindow& window,
        sf::Sprite sprite,
        float             offset,
        sf::Color         tint) const
    {
        const sf::Vector2f camCentre = window.getView().getCenter();
        const sf::Vector2f halfSize = sf::Vector2f(m_windowSize) * 0.5f;
        const float        texWidth = static_cast<float>(
            sprite.getTexture().getSize().x);

        sprite.setColor(tint);

        // Draw twice side by side for seamless loop
        sprite.setPosition(camCentre - halfSize + sf::Vector2f(offset, 0.f));
        window.draw(sprite);

        sprite.setPosition(camCentre - halfSize + sf::Vector2f(offset - texWidth, 0.f));
        window.draw(sprite);
    }

    float ParallaxLayer::wrapOffset(float offset, float width) const
    {
        if (width <= 0.f) return 0.f;
        return std::fmod(offset, width);
    }

}