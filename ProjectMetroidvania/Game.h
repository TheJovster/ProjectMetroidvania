#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "Input.h"
#include "TileMap.h"
#include "Player.h"
#include "Camera.h"
#include "DevMode.h"
#include "ParallaxLayer.h"
#include "LevelSerializer.h"
#include "TextureCache.h"

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

        TextureCache m_textureCache;
        Player  m_player;

        ParallaxLayer m_parallax;
        DevMode  m_devMode;
        LevelSerializer    m_serializer;

        bool     m_devModeActive = false;
        std::string        m_levelName = "test_level";

        void processEvents();
        void update(float dt);
        void render();
        void buildTestLevel();
        void saveLevel();
        void loadLevel();
    };

}