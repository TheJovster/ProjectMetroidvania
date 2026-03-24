#pragma once
#include <string>
#include "TileMap.h"
#include "ParallaxLayer.h"
#include "JsonWriter.h"
#include "JsonReader.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Metroidvania {

    // level metadata - name, dimensions, spawn point
    struct LevelData {
        std::string  name = "unnamed";
        int          columns = 40;
        int          rows = 20;
        sf::Vector2f playerSpawn = { 256.f, 500.f };

        // parallax config
        std::string  plxBkgPath = "";
        std::string  plxMidPath = "";
        sf::Color    bgColor = sf::Color(20, 20, 40);
    };

    class LevelSerializer {
    public:
        LevelSerializer() = default;

        // saves the level to assets/levels/levelJSONS/<name>.json
        bool save(
            const LevelData& data,
            const TileMap& bgLayer,
            const TileMap& mgLayer,
            const TileMap& fgLayer,
            const TileMap& parallaxFront
        );

        // loads a level by name from assets/levels/levelJSONS/<name>.json
        bool load(
            const std::string& name,
            LevelData& data,
            TileMap& bgLayer,
            TileMap& mgLayer,
            TileMap& fgLayer,
            TileMap& parallaxFront
        );

    private:
        // writes a single TileMap layer - only non-empty tiles
        void serializeLayer(JsonWriter& writer,
            const std::string& key,
            const TileMap& map);

        // reads a tile array back into a TileMap
        bool deserializeLayer(const JsonValue& layerArray,
            const std::string& layerName,
            TileMap& map);

        // converts TileType to int for storage
        int  tileTypeToInt(TileType type) const;

        // converts int back to TileType
        TileType intToTileType(int val) const;

        // builds the full output path from level name
        std::string buildPath(const std::string& name) const;

        // shows a windows message box and logs to cerr
        void showError(const std::string& context,
            const std::string& reason) const;
    };

}