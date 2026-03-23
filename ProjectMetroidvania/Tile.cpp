#include "Tile.h"

namespace Metroidvania {

    Tile::Tile(TileType type, sf::Vector2f position)
        : type(type)
    {
        shape.setSize(sf::Vector2f(k_tileSize, k_tileSize));
        shape.setPosition(position);

        switch (type)
        {
        case TileType::Stone:
            shape.setFillColor(sf::Color(100, 100, 100)); // gray
            shape.setOutlineColor(sf::Color(60, 60, 60)); // darker gray outline
            shape.setOutlineThickness(1.f);
            solid = true;
            break;

        case TileType::Platform:
            shape.setFillColor(sf::Color(140, 100, 60)); // brownish
            shape.setOutlineColor(sf::Color(100, 70, 40));
            shape.setOutlineThickness(1.f);
            solid = false; // handled separately - one way
            break;

        case TileType::Slope:
            shape.setFillColor(sf::Color(100, 100, 100));
            shape.setOutlineColor(sf::Color(60, 60, 60));
            shape.setOutlineThickness(1.f);
            solid = true;
            break;

        case TileType::Empty:
            solid = false;
            break;
        }
    }

    void Tile::draw(sf::RenderWindow& window) const
    {
        if (type != TileType::Empty)
            window.draw(shape);
    }

    sf::FloatRect Tile::getBounds() const
    {
        return shape.getGlobalBounds();
    }

}