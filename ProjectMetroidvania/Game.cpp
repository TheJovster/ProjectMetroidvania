#include "Game.h"

namespace Metroidvania {

    Game::Game()
        : m_tileMap(20, 12)
        , m_player(sf::Vector2f(256.f, 500.f))
    {
        // --- Window ---
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

        m_window.create(
            desktopMode,
            "Metroidvania",
            sf::Style::None
        );

        m_window.setFramerateLimit(60);
        m_window.setMouseCursorVisible(false);

        // --- World ---
        buildTestLevel();
    }

    // -------------------------------------------------------
    // Run
    // -------------------------------------------------------
    void Game::run()
    {
        while (m_window.isOpen())
        {
            const float dt = m_clock.restart().asSeconds();

            processEvents();
            update(dt);
            render();

            m_input.tick(); // age buffer, clear per-frame state - always last
        }
    }

    // -------------------------------------------------------
    // Events
    // -------------------------------------------------------
    void Game::processEvents()
    {
        while (const std::optional<sf::Event> event = m_window.pollEvent())
        {
            // Window close
            if (event->is<sf::Event::Closed>())
                m_window.close();

            // Feed everything to input
            m_input.update(*event);

            // Escape to exit
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    m_window.close();
        }
    }

    // -------------------------------------------------------
    // Update
    // -------------------------------------------------------
    void Game::update(float dt)
    {
        m_player.update(dt, m_input, m_tileMap);
    }

    // -------------------------------------------------------
    // Render
    // -------------------------------------------------------
    void Game::render()
    {
        m_window.clear(sf::Color::Black);

        m_tileMap.draw(m_window);
        m_player.draw(m_window);

        m_window.display();
    }

    // -------------------------------------------------------
    // Level
    // -------------------------------------------------------
    void Game::buildTestLevel()
    {
        // Floor at row 9 - leaves room above for jumping
        m_tileMap.buildFloor(9);

        // Left and right walls
        for (int row = 0; row < 9; ++row)
        {
            m_tileMap.setTile(0, row, TileType::Stone);
            m_tileMap.setTile(19, row, TileType::Stone);
        }

        // A platform to jump to
        m_tileMap.buildRow(6, 5, 8, TileType::Stone);

        // A higher platform
        m_tileMap.buildRow(4, 11, 14, TileType::Stone);
    }

}