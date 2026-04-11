#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"

namespace Metroidvania {

    enum class MainMenuAction {
        None,
        StartGame,
        Quit
    };

    class MainMenu {
    public:
        explicit MainMenu(sf::Vector2u windowSize, sf::Font& font);

        MainMenuAction update(const sf::Event& event, sf::Vector2f mousePos, bool mouseClicked);

        void draw(sf::RenderWindow& window) const;

        void reset();

    private:
        sf::RectangleShape m_background;
        sf::Text           m_title;

        //buttons - safe to add more here later (options, credits etc)
        Button m_startButton;   //remove this line to remove Start button
        Button m_quitButton;    //remove this line to remove Quit button

        int m_selectedIndex = 0;
        static constexpr int k_buttonCount = 2;

        void updateSelection(int dir);
        void confirmSelection(MainMenuAction& action);
    };

}