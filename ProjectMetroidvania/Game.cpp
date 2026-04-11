#include "Game.h"

namespace Metroidvania {

    static sf::Font loadFont()
    {
        sf::Font font;
        font.openFromFile("assets/fonts/default.ttf");
        return font;
    }

    Game::Game()
        : m_bgLayer(80, 20)
        , m_mgLayer(80, 20)
        , m_fgLayer(80, 20)
        , m_player(sf::Vector2f(256.f, 0.f), m_textureCache)
        , m_camera(sf::Vector2u(
            sf::VideoMode::getDesktopMode().size.x,
            sf::VideoMode::getDesktopMode().size.y))
        , m_parallax(
            sf::Vector2u(
                sf::VideoMode::getDesktopMode().size.x,
                sf::VideoMode::getDesktopMode().size.y),
            "assets/levels/parallax/parallax_background.png",
            "assets/levels/parallax/parallax_midground.png"
        )
        , m_devMode(m_bgLayer, m_mgLayer, m_fgLayer,
            m_parallax.getFrontLayer(),
            m_camera)
		, m_font(loadFont())
        , m_pauseMenu(sf::Vector2u(
            sf::VideoMode::getDesktopMode().size.x,
            sf::VideoMode::getDesktopMode().size.y), m_font)
        , m_mainMenu(sf::Vector2u(
            sf::VideoMode::getDesktopMode().size.x,
            sf::VideoMode::getDesktopMode().size.y), m_font)
    {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

        m_window.create(
            desktopMode,
            "Metroidvania",
            sf::Style::None
        );

        m_window.setFramerateLimit(60);
        m_window.setMouseCursorVisible(true);

        m_camera.setRoomBounds(sf::FloatRect(
            { 0.f, 0.f },
            { m_mgLayer.getWidth(), m_mgLayer.getHeight() }
        ));
        m_camera.setMode(CameraMode::Free);

        // try to load existing level - fall back to test level if not found
        const std::string levelPath = "assets/levels/levelJSONS/" + m_levelName + ".json";
        if (std::filesystem::exists(levelPath))
            loadLevel();
        else
            buildTestLevel();

        m_camera.snapTo(m_player.getPosition());

        auto doubleJump = std::make_shared<DoubleJumpAbility>();
        doubleJump->unlock();
        m_player.getAbilitySet().assign(AbilitySlot::DoubleJump, doubleJump);
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
                if (m_gameState == GameState::Playing)
                {
                    //escape pauses
                    if (key->code == sf::Keyboard::Key::Escape)
                    {
                        m_gameState = GameState::Paused;
                        m_pauseMenu.reset();
                        m_window.setMouseCursorVisible(true);

                    }

                    //F1 toggles devmode
                    if (key->code == sf::Keyboard::Key::F1)
                    {
                        m_devModeActive = !m_devModeActive;
                        m_devMode.setActive(m_devModeActive);
                        m_window.setMouseCursorVisible(m_devModeActive);
                    }

                    //ctrl+s saves
                    if (key->code == sf::Keyboard::Key::S)
                    {
                        const bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
                            || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                        if (ctrl) saveLevel();
                    }
                }
            }

            //pause menu events
            if (m_gameState == GameState::Paused)
            {
                const sf::Vector2f mousePos = sf::Vector2f(m_input.mousePosition());
                const bool mouseClicked = m_input.isMouseButtonPressed(sf::Mouse::Button::Left);

                const PauseAction action = m_pauseMenu.update(*event, mousePos, mouseClicked);
                std::cout << "PauseAction: " << (int)action << "\n";

                if (action == PauseAction::Resume)
                {
                    m_gameState = GameState::Playing;
                    m_window.setMouseCursorVisible(false);
                }
                else if (action == PauseAction::QuitToMenu)
                {
                    resetGame();
                    m_gameState = GameState::MainMenu;
                    m_mainMenu.reset();
                    m_window.setMouseCursorVisible(true);
                }
            }

            //main menu events
            else if (m_gameState == GameState::MainMenu)
            {
                const sf::Vector2f mousePos = sf::Vector2f(m_input.mousePosition());
                const bool mouseClicked = m_input.isMouseButtonPressed(sf::Mouse::Button::Left);

                const MainMenuAction action = m_mainMenu.update(*event, mousePos, mouseClicked);

                if (action == MainMenuAction::StartGame)
                {
					resetGame();
                    m_gameState = GameState::Playing;
                    m_window.setMouseCursorVisible(false);
                }
                else if (action == MainMenuAction::Quit)
                    m_window.close();
            }

            if (m_devModeActive)
                m_devMode.handleEvent(*event);
        }
    }

    void Game::update(float dt)
    {
        if (m_gameState != GameState::Playing) return;

        if (!m_devModeActive)
        {
            m_player.update(dt, m_input, m_mgLayer);
            m_camera.update(dt, m_player.getPosition(),
                m_player.getAnimator().isFacingRight());

            for (auto& enemy : m_enemies)
                enemy.update(dt, m_mgLayer, m_player.getPosition());

            m_enemies.erase(
                std::remove_if(m_enemies.begin(), m_enemies.end(),
                    [](const Enemy& e) { return !e.isAlive(); }),
                m_enemies.end());

            const auto hitbox = m_player.getAttackHitbox();
            if (hitbox.has_value() && !m_player.hasHit())
            {
                for (auto& enemy : m_enemies)
                {
                    if (!enemy.isAlive()) continue;
                    if (hitbox->findIntersection(enemy.getBounds()).has_value())
                    {
                        enemy.takeDamage(1);
                        m_player.setHasHit(true);
                        break;
                    }
                }
            }

            m_hud.update(m_player.getHealth(), m_player.getMaxHealth(), 0, 1);
        }
        else
        {
            m_devMode.update(dt, m_window);
        }

        m_parallax.update(m_camera.getPosition());
    }

    void Game::render()
    {
        m_window.clear(sf::Color::Black);

        if (m_gameState == GameState::MainMenu)
        {
            sf::View defaultView = m_window.getDefaultView();
            m_window.setView(defaultView);
            m_mainMenu.draw(m_window);
            m_window.display();
            return;
        }

        //playing or paused - draw game world
        m_camera.apply(m_window);

        m_parallax.draw(m_window);
        m_bgLayer.draw(m_window);
        m_mgLayer.draw(m_window);

        for (auto& enemy : m_enemies)
            enemy.draw(m_window);

        m_player.draw(m_window);
        m_fgLayer.draw(m_window);
        m_devMode.drawWorld(m_window);

        //screen space
        sf::View defaultView = m_window.getDefaultView();
        m_window.setView(defaultView);

        m_devMode.drawHUD(m_window);
        m_hud.draw(m_window);

        //pause overlay drawn on top of everything
        if (m_gameState == GameState::Paused)
            m_pauseMenu.draw(m_window);

        m_camera.apply(m_window);
        m_window.display();
    }

    void Game::buildTestLevel()
    {
        // floor across the whole map
        m_mgLayer.buildFloor(17);

        // left and right walls
        for (int row = 0; row < 17; ++row)
        {
            m_mgLayer.setTile(0, row, TileType::Stone);
            m_mgLayer.setTile(79, row, TileType::Stone);
        }

        // platforms - spread across the wide level
        m_mgLayer.buildRow(14, 5, 10, TileType::Stone);
        m_mgLayer.buildRow(11, 15, 22, TileType::Stone);
        m_mgLayer.buildRow(13, 25, 30, TileType::Stone);
        m_mgLayer.buildRow(10, 35, 42, TileType::Stone);
        m_mgLayer.buildRow(12, 48, 54, TileType::Stone);
        m_mgLayer.buildRow(9, 58, 65, TileType::Stone);
        m_mgLayer.buildRow(14, 68, 74, TileType::Stone);

        // some elevated sections to break up the run
        m_mgLayer.buildRow(15, 20, 24, TileType::Stone);
        m_mgLayer.buildRow(15, 44, 48, TileType::Stone);
        m_mgLayer.buildRow(15, 62, 66, TileType::Stone);

        m_enemies.emplace_back(sf::Vector2f(600.f, 300.f));

    }

    void Game::saveLevel()
    {
        LevelData data;
        data.name = m_levelName;
        data.columns = m_mgLayer.getColumns();
        data.rows = m_mgLayer.getRows();
        data.playerSpawn = m_player.getPosition();
        data.plxBkgPath = "";
        data.plxMidPath = "";
        data.bgColor = sf::Color(20, 20, 40);

        const bool ok = m_serializer.save(
            data,
            m_bgLayer,
            m_mgLayer,
            m_fgLayer,
            m_parallax.getFrontLayer()
        );

        if (ok)
            std::cout << "[Game] Level saved: " << m_levelName << "\n";
    }

    void Game::loadLevel()
    {
        LevelData data;

        const bool ok = m_serializer.load(
            m_levelName,
            data,
            m_bgLayer,
            m_mgLayer,
            m_fgLayer,
            m_parallax.getFrontLayer()
        );

        if (ok)
        {
            m_player.setPosition(data.playerSpawn);
            std::cout << "[Game] Level loaded: " << m_levelName << "\n";
        }
    }

    void Game::resetGame()
    {
        // clear enemies
        m_enemies.clear();

        // clear tile layers
        m_bgLayer = TileMap(80, 20);
        m_mgLayer = TileMap(80, 20);
        m_fgLayer = TileMap(80, 20);

        // reset player position
        m_player.setPosition(sf::Vector2f(256.f, 0.f));

        // rebuild level
        buildTestLevel();

        // snap camera to player
        m_camera.snapTo(m_player.getPosition());
    }

}