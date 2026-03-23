#include "Game.h"

namespace Metroidvania {

    Game::Game()
        : m_bgLayer(40, 20)
        , m_mgLayer(40, 20)
        , m_fgLayer(40, 20)
        , m_player(sf::Vector2f(256.f, 500.f))
        , m_camera(sf::Vector2u(
            sf::VideoMode::getDesktopMode().size.x,
            sf::VideoMode::getDesktopMode().size.y))
        , m_devMode(m_bgLayer, m_mgLayer, m_fgLayer, m_camera)
    {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

        m_window.create(
            desktopMode,
            "Metroidvania",
            sf::Style::None
        );

        m_window.setFramerateLimit(60);
        m_window.setMouseCursorVisible(false);

        m_camera.setRoomBounds(sf::FloatRect(
            { 0.f, 0.f },
            { m_mgLayer.getWidth(), m_mgLayer.getHeight() }
        ));
        m_camera.setMode(CameraMode::Free);

        buildTestLevel();

        m_camera.snapTo(m_player.getPosition());
    }

    void Game::run()
    {
        while (m_window.isOpen())
        {
            const float dt = m_clock.restart().asSeconds();

            processEvents();
            update(dt);
            render();

            m_input.tick();
        }
    }

    void Game::processEvents()
    {
        while (const std::optional<sf::Event> event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                m_window.close();

            m_input.update(*event);

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    m_window.close();

                // Toggle DevMode
                if (key->code == sf::Keyboard::Key::F1)
                {
                    m_devModeActive = !m_devModeActive;
                    m_devMode.setActive(m_devModeActive);
                    m_window.setMouseCursorVisible(m_devModeActive);
                }
            }

            // Feed events to DevMode when active
            if (m_devModeActive)
                m_devMode.handleEvent(*event);
        }
    }

    void Game::update(float dt)
    {
        if (!m_devModeActive)
        {
            m_player.update(dt, m_input, m_mgLayer);
            m_camera.update(
                dt,
                m_player.getPosition(),
                m_player.getAnimator().isFacingRight()
            );
        }
        else
        {
            m_devMode.update(dt, m_window);
        }
    }

    void Game::render()
    {
        m_camera.apply(m_window);
        m_window.clear(sf::Color::Black);

        // World space - camera view active
        m_bgLayer.draw(m_window);
        m_mgLayer.draw(m_window);
        m_player.draw(m_window);
        m_fgLayer.draw(m_window);
        m_devMode.drawWorld(m_window);

        // Screen space - reset to default view for HUD
        m_devMode.drawHUD(m_window);

        // Reapply camera view after HUD so nothing else draws in screen space
        m_camera.apply(m_window);

        m_window.display();
    }

    void Game::buildTestLevel()
    {
        m_mgLayer.buildFloor(17);

        for (int row = 0; row < 17; ++row)
        {
            m_mgLayer.setTile(0, row, TileType::Stone);
            m_mgLayer.setTile(39, row, TileType::Stone);
        }

        m_mgLayer.buildRow(14, 5, 10, TileType::Stone);
        m_mgLayer.buildRow(11, 15, 22, TileType::Stone);
        m_mgLayer.buildRow(8, 25, 32, TileType::Stone);
    }

}