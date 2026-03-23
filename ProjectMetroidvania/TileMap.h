#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"

namespace Metroidvania {

    class TileMap {
    public:
        TileMap(int columns, int rows);

        // --- Building the map ---
        void setTile(int col, int row, TileType type);
        void buildFloor(int row);                        // fills an entire row with Stone
        void buildRow(int row, int colStart, int colEnd, TileType type);

        // --- Collision ---
        // Returns all solid tiles that overlap the given rect
        std::vector<sf::FloatRect> getSolidBounds(const sf::FloatRect& area) const;

        // Platform tiles - one way, only block from above
        std::vector<sf::FloatRect> getPlatformBounds(const sf::FloatRect& area) const;

        // --- Draw ---
        void draw(sf::RenderWindow& window) const;

        // --- Queries ---
        int getColumns() const { return m_columns; }
        int getRows()    const { return m_rows; }
        float getWidth() const { return m_columns * k_tileSize; }
        float getHeight()const { return m_rows * k_tileSize; }

    private:
        int m_columns;
        int m_rows;

        // Flat array - access via [row * m_columns + col]
        std::vector<Tile> m_tiles;

        // Bounds check before accessing m_tiles
        bool isValid(int col, int row) const;
        int  index(int col, int row) const { return row * m_columns + col; }
    };

}