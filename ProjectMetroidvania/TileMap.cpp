#include "TileMap.h"

namespace Metroidvania {

    TileMap::TileMap(int columns, int rows)
        : m_columns(columns)
        , m_rows(rows)
    {
        // Fill with empty tiles at correct positions
        m_tiles.reserve(columns * rows);

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < columns; ++col)
            {
                sf::Vector2f position(col * k_tileSize, row * k_tileSize);
                m_tiles.emplace_back(TileType::Empty, position);
            }
        }
    }

    void TileMap::setTile(int col, int row, TileType type)
    {
        if (!isValid(col, row)) return;

        sf::Vector2f position(col * k_tileSize, row * k_tileSize);
        m_tiles[index(col, row)] = Tile(type, position);
    }

    void TileMap::buildFloor(int row)
    {
        for (int col = 0; col < m_columns; ++col)
            setTile(col, row, TileType::Stone);
    }

    void TileMap::buildRow(int row, int colStart, int colEnd, TileType type)
    {
        for (int col = colStart; col <= colEnd; ++col)
            setTile(col, row, type);
    }

    std::vector<sf::FloatRect> TileMap::getSolidBounds(const sf::FloatRect& area) const
    {
        std::vector<sf::FloatRect> result;

        // Determine tile range that overlaps the query area
        int colStart = static_cast<int>(area.position.x / k_tileSize);
        int colEnd = static_cast<int>((area.position.x + area.size.x) / k_tileSize);
        int rowStart = static_cast<int>(area.position.y / k_tileSize);
        int rowEnd = static_cast<int>((area.position.y + area.size.y) / k_tileSize);

        // Clamp to map bounds
        colStart = std::max(colStart, 0);
        colEnd = std::min(colEnd, m_columns - 1);
        rowStart = std::max(rowStart, 0);
        rowEnd = std::min(rowEnd, m_rows - 1);

        for (int row = rowStart; row <= rowEnd; ++row)
        {
            for (int col = colStart; col <= colEnd; ++col)
            {
                const Tile& tile = m_tiles[index(col, row)];
                if (tile.solid)
                    result.push_back(tile.getBounds());
            }
        }

        return result;
    }

    std::vector<sf::FloatRect> TileMap::getPlatformBounds(const sf::FloatRect& area) const
    {
        std::vector<sf::FloatRect> result;

        int colStart = static_cast<int>(area.position.x / k_tileSize);
        int colEnd = static_cast<int>((area.position.x + area.size.x) / k_tileSize);
        int rowStart = static_cast<int>(area.position.y / k_tileSize);
        int rowEnd = static_cast<int>((area.position.y + area.size.y) / k_tileSize);

        colStart = std::max(colStart, 0);
        colEnd = std::min(colEnd, m_columns - 1);
        rowStart = std::max(rowStart, 0);
        rowEnd = std::min(rowEnd, m_rows - 1);

        for (int row = rowStart; row <= rowEnd; ++row)
        {
            for (int col = colStart; col <= colEnd; ++col)
            {
                const Tile& tile = m_tiles[index(col, row)];
                if (tile.type == TileType::Platform)
                    result.push_back(tile.getBounds());
            }
        }

        return result;
    }

    void TileMap::draw(sf::RenderWindow& window) const
    {
        for (const Tile& tile : m_tiles)
            tile.draw(window);
    }

    bool TileMap::isValid(int col, int row) const
    {
        return col >= 0 && col < m_columns
            && row >= 0 && row < m_rows;
    }

}