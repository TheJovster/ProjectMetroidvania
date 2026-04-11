#include "Button.h"

namespace Metroidvania {

    Button::Button(const std::string& label, sf::Vector2f position, sf::Vector2f size, sf::Font& font)
        : m_text(font, label, 24)
    {
        m_background.setSize(size);
        m_background.setPosition(position);
        m_background.setFillColor(k_normalColor);

        //centre text in button
        const sf::FloatRect textBounds = m_text.getLocalBounds();
        m_text.setOrigin(sf::Vector2f(
            textBounds.position.x + textBounds.size.x * 0.5f,
            textBounds.position.y + textBounds.size.y * 0.5f
        ));
        m_text.setPosition(sf::Vector2f(
            position.x + size.x * 0.5f,
            position.y + size.y * 0.5f
        ));
        m_text.setFillColor(k_textNormal);
    }

    void Button::update(sf::Vector2f mousePos, bool mouseClicked)
    {
        m_confirmed = false;

        //mouse hover check
        m_hovered = m_background.getGlobalBounds().contains(mousePos);

        //mouse click confirm
        if (m_hovered && mouseClicked)
            m_confirmed = true;

        //update color
        if (m_selected || m_hovered)
        {
            m_background.setFillColor(m_selected ? k_selectedColor : k_hoverColor);
            m_text.setFillColor(k_textSelected);
        }
        else
        {
            m_background.setFillColor(k_normalColor);
            m_text.setFillColor(k_textNormal);
        }
    }

    void Button::draw(sf::RenderWindow& window) const
    {
        window.draw(m_background);
        window.draw(m_text);
    }

    void Button::setSelected(bool selected)
    {
        m_selected = selected;
    }

}