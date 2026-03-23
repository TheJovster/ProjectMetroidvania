#include "Game.h"

namespace Metroidvania {

    Game::Game()
        : m_tileMap(40, 20)
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
            { m_tileMap.getWidth(), m_tileMap.getHeight() }
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
        m_tileMap.buildFloor(17);

        for (int row = 0; row < 17; ++row)
        {
            m_tileMap.setTile(0, row, TileType::Stone);
            m_tileMap.setTile(39, row, TileType::Stone);
        }

        m_tileMap.buildRow(14, 5, 10, TileType::Stone);
        m_tileMap.buildRow(11, 15, 22, TileType::Stone);
        m_tileMap.buildRow(8, 25, 32, TileType::Stone);
    }

}