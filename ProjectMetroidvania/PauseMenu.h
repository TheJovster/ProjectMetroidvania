#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"

namespace Metroidvania {

    enum class PauseAction {
        None,
        Resume,
        QuitToMenu
    };

    class PauseMenu {
    public:
        explicit PauseMenu(sf::Vector2u windowSize, sf::Font& font);

        //update - returns action if one was triggered
        PauseAction update(const sf::Event& event, sf::Vector2f mousePos, bool mouseClicked);

        void draw(sf::RenderWindow& window) const;

        //reset selection state when menu opens
        void reset();

    private:
        sf::RectangleShape m_overlay;

        //buttons - safe to add more here later for inventory etc
        Button m_resumeButton;      //remove this line to remove Resume button
        Button m_quitButton;        //remove this line to remove Quit to Menu button

        int m_selectedIndex = 0;    //0 = Resume, 1 = Quit
        static constexpr int k_buttonCount = 2;

        void updateSelection(int dir);
        void confirmSelection(PauseAction& action);
    };

}