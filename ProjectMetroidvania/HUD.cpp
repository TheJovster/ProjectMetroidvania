#include "HUD.h"

namespace Metroidvania {

    HUD::HUD()
    {
        buildBackground(m_hpBackground, k_hudHpBarY);
        buildBackground(m_mpBackground, k_hudMpBarY);

        //hp fill - red
        m_hpFill.setPosition(sf::Vector2f(k_hudBarX + k_hudBarPadding, k_hudHpBarY + k_hudBarPadding));
        m_hpFill.setFillColor(sf::Color(200, 40, 40));

        //mp fill - blue
        m_mpFill.setPosition(sf::Vector2f(k_hudBarX + k_hudBarPadding, k_hudMpBarY + k_hudBarPadding));
        m_mpFill.setFillColor(sf::Color(40, 80, 200));
    }

    void HUD::update(int currentHp, int maxHp, int currentMp, int maxMp)
    {
        m_maxHp = maxHp;
        m_maxMp = maxMp;

        updateFill(m_hpFill, currentHp, maxHp, k_hudHpBarY, sf::Color(200, 40, 40));
        updateFill(m_mpFill, currentMp, maxMp, k_hudMpBarY, sf::Color(40, 80, 200));
    }

    void HUD::draw(sf::RenderWindow& window) const
    {
        window.draw(m_hpBackground);
        window.draw(m_hpFill);
        window.draw(m_mpBackground);
        window.draw(m_mpFill);
    }

    void HUD::buildBackground(sf::RectangleShape& bg, float y)
    {
        bg.setSize(sf::Vector2f(k_hudBarWidth, k_hudBarHeight));
        bg.setPosition(sf::Vector2f(k_hudBarX, y));
        bg.setFillColor(sf::Color(20, 20, 20, 200));
    }

    void HUD::updateFill(sf::RectangleShape& fill, int current, int max, float y, sf::Color color)
    {
        if (max <= 0) return;

        const float fillWidth = (static_cast<float>(current) / static_cast<float>(max))
            * (k_hudBarWidth - k_hudBarPadding * 2.f);

        fill.setSize(sf::Vector2f(std::max(0.f, fillWidth), k_hudBarHeight - k_hudBarPadding * 2.f));
        fill.setFillColor(color);
    }

}