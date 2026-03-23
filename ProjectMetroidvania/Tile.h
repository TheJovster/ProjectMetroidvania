#pragma once
#include <SFML/Graphics.hpp>

namespace Metroidvania {

    // Environment tiles are 128x128
    constexpr float k_tileSize = 128.f;

    enum class TileType {
        Stone,      // standard floor/wall
        Platform,   // one-way platform - can jump through from below
        Slope,      // reserved for later
        Empty       // no collision, no draw
    };

    struct Tile {
        TileType          type = TileType::Empty;
        sf::RectangleShape shape;
        bool              solid = false;

        Tile() = default;
        Tile(TileType type, sf::Vector2f position);

        void draw(sf::RenderWindow& window) const;
        sf::FloatRect getBounds() const;
    };

}