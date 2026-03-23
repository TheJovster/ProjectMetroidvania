#include "DevMode.h"
#include <cmath>

namespace Metroidvania {

    DevMode::DevMode(TileMap& bg, TileMap& mg, TileMap& fg, Camera& camera)
        : m_camera(camera)
    {
        m_layers[0] = &bg;
        m_layers[1] = &mg;
        m_layers[2] = &fg;

        m_ghost.setSize(sf::Vector2f(k_tileSize, k_tileSize));
        m_ghost.setFillColor(sf::Color(255, 255, 255, 60));
        m_ghost.setOutlineColor(sf::Color(255, 255, 255, 180));
        m_ghost.setOutlineThickness(1.f);

        // Try to load a font - HUD won't draw if this fails
        m_fontLoaded = m_font.openFromFile("assets/fonts/default.ttf");
    }

    void DevMode::setActive(bool active)
    {
        m_active = active;

        if (active)
            m_camera.setMode(CameraMode::Locked);
        else
            m_camera.setMode(CameraMode::Free);
    }

    void DevMode::update(float dt, const sf::RenderWindow& window)
    {
        if (!m_active) return;

        updateCameraMovement(dt);

        sf::Vector2f worldPos = mouseWorldPos(window);
        updateGhost(worldPos);

        if (m_painting) paintAt(worldPos);
        if (m_erasing)  eraseAt(worldPos);
    }

    void DevMode::handleEvent(const sf::Event& event)
    {
        if (!m_active) return;

        // Cycle layer - Tab forward, Shift+Tab backward
        if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Tab)
            {
                const bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
                cycleLayer(shift ? -1 : 1);
            }
        }

        // Cycle tile type - scroll wheel
        if (const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>())
            cycleTile(scroll->delta > 0.f ? 1 : -1);

        // Paint / erase
        if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mb->button == sf::Mouse::Button::Left)  m_painting = true;
            if (mb->button == sf::Mouse::Button::Right) m_erasing = true;
        }

        if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>())
        {
            if (mb->button == sf::Mouse::Button::Left)  m_painting = false;
            if (mb->button == sf::Mouse::Button::Right) m_erasing = false;
        }
    }

    void DevMode::drawWorld(sf::RenderWindow& window) const
    {
        if (!m_active) return;
        window.draw(m_ghost);
    }

    void DevMode::drawHUD(sf::RenderWindow& window) const
    {
        if (!m_active || !m_fontLoaded) return;

        // Reset to default view so HUD is always screen space
        sf::View defaultView = window.getDefaultView();
        window.setView(defaultView);

        // Layer indicator
        sf::Text layerText(m_font, "Layer: " + layerName(), 18);
        layerText.setFillColor(sf::Color::White);
        layerText.setPosition(sf::Vector2f(20.f, 20.f));
        window.draw(layerText);

        // Tile indicator
        sf::Text tileText(m_font, "Tile: " + tileName(), 18);
        tileText.setFillColor(sf::Color::White);
        tileText.setPosition(sf::Vector2f(20.f, 46.f));
        window.draw(tileText);

        // DevMode active indicator
        sf::Text devText(m_font, "[DEV MODE]", 18);
        devText.setFillColor(sf::Color(255, 80, 80));
        devText.setPosition(sf::Vector2f(20.f, 72.f));
        window.draw(devText);
    }

    sf::Vector2f DevMode::mouseWorldPos(const sf::RenderWindow& window) const
    {
        const sf::Vector2i mousePixels = sf::Mouse::getPosition(window);
        return window.mapPixelToCoords(mousePixels, m_camera.getView());
    }

    void DevMode::paintAt(sf::Vector2f worldPos)
    {
        const int col = static_cast<int>(worldPos.x / k_tileSize);
        const int row = static_cast<int>(worldPos.y / k_tileSize);
        activeLayer()->setTile(col, row, activeTileType());
    }

    void DevMode::eraseAt(sf::Vector2f worldPos)
    {
        const int col = static_cast<int>(worldPos.x / k_tileSize);
        const int row = static_cast<int>(worldPos.y / k_tileSize);
        activeLayer()->setTile(col, row, TileType::Empty);
    }

    void DevMode::cycleLayer(int dir)
    {
        const int count = static_cast<int>(EditorLayer::COUNT);
        m_layerIndex = (m_layerIndex + dir + count) % count;
    }

    void DevMode::cycleTile(int dir)
    {
        const int count = static_cast<int>(k_editableTiles.size());
        m_tileIndex = (m_tileIndex + dir + count) % count;
    }

    void DevMode::updateGhost(sf::Vector2f worldPos)
    {
        // Snap ghost to tile grid
        const float snappedX = std::floor(worldPos.x / k_tileSize) * k_tileSize;
        const float snappedY = std::floor(worldPos.y / k_tileSize) * k_tileSize;
        m_ghost.setPosition(sf::Vector2f(snappedX, snappedY));

        // Tint ghost by active tile type so you can see what you're placing
        switch (activeTileType())
        {
        case TileType::Stone:    m_ghost.setFillColor(sf::Color(150, 150, 150, 80)); break;
        case TileType::Platform: m_ghost.setFillColor(sf::Color(140, 100, 60, 80)); break;
        case TileType::Slope:    m_ghost.setFillColor(sf::Color(100, 150, 100, 80)); break;
        case TileType::Empty:    m_ghost.setFillColor(sf::Color(255, 60, 60, 80)); break;
        }
    }

    void DevMode::updateCameraMovement(float dt)
    {
        // WASD moves the camera freely in editor mode
        sf::Vector2f dir(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dir.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dir.x += 1.f;

        if (dir.x != 0.f || dir.y != 0.f)
        {
            // Normalise so diagonal isn't faster
            const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            dir /= len;

            const sf::Vector2f current = m_camera.getPosition();
            m_camera.snapTo(current + dir * k_cameraEditorSpeed * dt);
        }
    }

    std::string DevMode::layerName() const
    {
        switch (static_cast<EditorLayer>(m_layerIndex))
        {
        case EditorLayer::Background: return "Background";
        case EditorLayer::Midground:  return "Midground";
        case EditorLayer::Foreground: return "Foreground";
        default:                      return "Unknown";
        }
    }

    std::string DevMode::tileName() const
    {
        switch (activeTileType())
        {
        case TileType::Stone:    return "Stone";
        case TileType::Platform: return "Platform";
        case TileType::Slope:    return "Slope";
        case TileType::Empty:    return "Erase";
        default:                 return "Unknown";
        }
    }

}