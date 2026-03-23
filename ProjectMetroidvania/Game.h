#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Input.h"
#include "TileMap.h"
#include "Player.h"
#include "Camera.h"
#include "DevMode.h"

namespace Metroidvania {

    class Game {
    public:
        Game();
        ~Game() = default;

        void run();

    private:
        sf::RenderWindow m_window;
        sf::Clock        m_clock;

        Input   m_input;
        Camera  m_camera;

        TileMap  m_bgLayer;
        TileMap  m_mgLayer;
        TileMap  m_fgLayer;

        DevMode  m_devMode;
        bool     m_devModeActive = false;

        Player  m_player;

        void processEvents();
        void update(float dt);
        void render();

        void buildTestLevel();
    };

}