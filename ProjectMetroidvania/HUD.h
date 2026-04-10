#pragma once
#include <SFML/Graphics.hpp>

namespace Metroidvania {

    //HUD layout constants - tune as needed
    constexpr float k_hudBarWidth = 200.f;
    constexpr float k_hudBarHeight = 16.f;
    constexpr float k_hudBarX = 20.f;
    constexpr float k_hudHpBarY = 20.f;
    constexpr float k_hudMpBarY = 44.f;
    constexpr float k_hudBarPadding = 2.f;   // gap between background and fill

    class HUD {
    public:
        HUD();

        //called by Game when values change
        void update(int currentHp, int maxHp, int currentMp, int maxMp);

        //draw in screen space - call after camera view is reset
        void draw(sf::RenderWindow& window) const;

    private:
        //HP bar
        sf::RectangleShape m_hpBackground;
        sf::RectangleShape m_hpFill;

        //MP bar
        sf::RectangleShape m_mpBackground;
        sf::RectangleShape m_mpFill;

        //cached max values for fill calculation
        int m_maxHp = 1;
        int m_maxMp = 1;

        void buildBackground(sf::RectangleShape& bg, float y);
        void updateFill(sf::RectangleShape& fill, int current, int max, float y, sf::Color color);
    };

}