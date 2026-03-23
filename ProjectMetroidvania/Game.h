#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Input.h"
#include "TileMap.h"
#include "Player.h"

namespace Metroidvania {

    class Game {
    public:
        Game();
        ~Game() = default;

        void run();

    private:
        // --- Core ---
        sf::RenderWindow m_window;
        sf::Clock        m_clock;

        // --- Systems ---
        Input   m_input;

        // --- World ---
        TileMap m_tileMap;
        Player  m_player;

        // --- Game loop ---
        void processEvents();
        void update(float dt);
        void render();

        // --- Setup ---
        void buildTestLevel();
    };

}