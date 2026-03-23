#include "Game.h"

namespace Metroidvania {

    Game::Game()
        : m_tileMap(20, 12)
        , m_player(sf::Vector2f(256.f, 500.f))
        , m_camera(sf::Vector2u(
            sf::VideoMode::getDesktopMode().size.x,
            sf::VideoMode::getDesktopMode().size.y))
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
            { 20 * k_tileSize, 12 * k_tileSize }
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

            m_input.tick(); //age buffer - always last
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
                if (key->code == sf::Keyboard::Key::Escape)
                    m_window.close();
        }
    }


    void Game::update(float dt)
    {
        m_player.update(dt, m_input, m_tileMap);

        m_camera.update(
            dt,
            m_player.getPosition(),
            m_player.getAnimator().isFacingRight()
        );
    }

    void Game::render()
    {
        m_camera.apply(m_window);

        m_window.clear(sf::Color::Black);

        m_tileMap.draw(m_window);
        m_player.draw(m_window);

        m_window.display();
    }


    void Game::buildTestLevel()
    {
        m_tileMap.buildFloor(9);

        for (int row = 0; row < 9; ++row)
        {
            m_tileMap.setTile(0, row, TileType::Stone);
            m_tileMap.setTile(19, row, TileType::Stone);
        }

        m_tileMap.buildRow(6, 5, 8, TileType::Stone);
        m_tileMap.buildRow(4, 11, 14, TileType::Stone);
    }

}