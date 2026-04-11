#pragma once
#include <SFML/Graphics.hpp>
#include <string>



namespace Metroidvania 
{
	class Button 
	{
	public:
		Button(const std::string& label, sf::Vector2f position, sf::Vector2f size, sf::Font& font);

		void update(sf::Vector2f mousePos, bool mouseClicked);
		void draw(sf::RenderWindow& window) const;

		void setSelected(bool selected);
		bool isSelected() const { return m_selected; }

		bool isConfirmed() const { return m_confirmed; }
		void resetConfirmed() { m_confirmed = false; }

	private:

		sf::RectangleShape m_background;
		sf::Text m_text;

		bool m_selected = false;
		bool m_confirmed = false;
		bool m_hovered = false;

		//colors
		static constexpr sf::Color k_normalColor = sf::Color(40, 40, 40, 200);
		static constexpr sf::Color k_selectedColor = sf::Color(80, 80, 80, 200);
		static constexpr sf::Color k_hoverColor = sf::Color(60, 60, 60, 200);
		static constexpr sf::Color k_textNormal = sf::Color(180, 180, 180, 255);
		static constexpr sf::Color k_textSelected = sf::Color(255, 255, 255, 255);
	};
}

