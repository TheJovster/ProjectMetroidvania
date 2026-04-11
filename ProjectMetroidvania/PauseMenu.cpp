#include "PauseMenu.h"
#include <iostream>

namespace Metroidvania {

    PauseMenu::PauseMenu(sf::Vector2u windowSize, sf::Font& font)
        : m_resumeButton("Resume",
            sf::Vector2f(windowSize.x * 0.5f - 100.f, windowSize.y * 0.5f - 60.f),
            sf::Vector2f(200.f, 50.f), font)
        , m_quitButton("Quit to Menu",
            sf::Vector2f(windowSize.x * 0.5f - 100.f, windowSize.y * 0.5f + 20.f),
            sf::Vector2f(200.f, 50.f), font)
    {
        //semi-transparent dark overlay covers the whole screen
        m_overlay.setSize(sf::Vector2f(windowSize));
        m_overlay.setFillColor(sf::Color(0, 0, 0, 160));

        reset();
    }

    PauseAction PauseMenu::update(const sf::Event& event, sf::Vector2f mousePos, bool mouseClicked)
    {
        PauseAction action = PauseAction::None;

        //keyboard navigation
        if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Up)
                updateSelection(-1);
            else if (key->code == sf::Keyboard::Key::Down)
                updateSelection(1);
            else if (key->code == sf::Keyboard::Key::Enter)
                confirmSelection(action);
        }

        //update buttons with mouse state
        m_resumeButton.update(mousePos, mouseClicked);
        m_quitButton.update(mousePos, mouseClicked);

        //sync keyboard selection highlight
        m_resumeButton.setSelected(m_selectedIndex == 0);
        m_quitButton.setSelected(m_selectedIndex == 1);

        //mouse confirm
        if (m_resumeButton.isConfirmed()) action = PauseAction::Resume;
        if (m_quitButton.isConfirmed())   action = PauseAction::QuitToMenu;

        m_resumeButton.resetConfirmed();
        m_quitButton.resetConfirmed();

        return action;
    }

    void PauseMenu::draw(sf::RenderWindow& window) const
    {
        window.draw(m_overlay);
        m_resumeButton.draw(window);
        m_quitButton.draw(window);
    }

    void PauseMenu::reset()
    {
        m_selectedIndex = 0;
        m_resumeButton.setSelected(true);
        m_quitButton.setSelected(false);
    }

    void PauseMenu::updateSelection(int dir)
    {
        m_selectedIndex = (m_selectedIndex + dir + k_buttonCount) % k_buttonCount;
        std::cout << "selectedIndex: " << m_selectedIndex << "\n";

    }

    void PauseMenu::confirmSelection(PauseAction& action)
    {
        if (m_selectedIndex == 0) action = PauseAction::Resume;
        if (m_selectedIndex == 1) action = PauseAction::QuitToMenu;
    }

}