#include "MainMenu.h"

namespace Metroidvania {

    MainMenu::MainMenu(sf::Vector2u windowSize, sf::Font& font)
        : m_title(font, "METROIDVANIA", 64)
        , m_startButton("Start Game",
            sf::Vector2f(windowSize.x * 0.5f - 100.f, windowSize.y * 0.5f - 60.f),
            sf::Vector2f(200.f, 50.f), font)
        , m_quitButton("Quit",
            sf::Vector2f(windowSize.x * 0.5f - 100.f, windowSize.y * 0.5f + 20.f),
            sf::Vector2f(200.f, 50.f), font)
    {
        //background - solid dark
        m_background.setSize(sf::Vector2f(windowSize));
        m_background.setFillColor(sf::Color(10, 10, 20));

        //title - centred
        const sf::FloatRect titleBounds = m_title.getLocalBounds();
        m_title.setOrigin(sf::Vector2f(
            titleBounds.position.x + titleBounds.size.x * 0.5f,
            titleBounds.position.y + titleBounds.size.y * 0.5f
        ));
        m_title.setPosition(sf::Vector2f(windowSize.x * 0.5f, windowSize.y * 0.3f));
        m_title.setFillColor(sf::Color::White);

        reset();
    }

    MainMenuAction MainMenu::update(const sf::Event& event, sf::Vector2f mousePos, bool mouseClicked)
    {
        MainMenuAction action = MainMenuAction::None;

        if (const auto* key = event.getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Up)
                updateSelection(-1);
            else if (key->code == sf::Keyboard::Key::Down)
                updateSelection(1);
            else if (key->code == sf::Keyboard::Key::Enter)
                confirmSelection(action);
        }

        m_startButton.update(mousePos, mouseClicked);
        m_quitButton.update(mousePos, mouseClicked);

        m_startButton.setSelected(m_selectedIndex == 0);
        m_quitButton.setSelected(m_selectedIndex == 1);

        if (m_startButton.isConfirmed()) action = MainMenuAction::StartGame;
        if (m_quitButton.isConfirmed())  action = MainMenuAction::Quit;

        m_startButton.resetConfirmed();
        m_quitButton.resetConfirmed();

        return action;
    }

    void MainMenu::draw(sf::RenderWindow& window) const
    {
        window.draw(m_background);
        window.draw(m_title);
        m_startButton.draw(window);
        m_quitButton.draw(window);
    }

    void MainMenu::reset()
    {
        m_selectedIndex = 0;
        m_startButton.setSelected(true);
        m_quitButton.setSelected(false);
    }

    void MainMenu::updateSelection(int dir)
    {
        m_selectedIndex = (m_selectedIndex + dir + k_buttonCount) % k_buttonCount;
    }

    void MainMenu::confirmSelection(MainMenuAction& action)
    {
        if (m_selectedIndex == 0) action = MainMenuAction::StartGame;
        if (m_selectedIndex == 1) action = MainMenuAction::Quit;
    }

}