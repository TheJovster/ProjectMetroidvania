#include "LevelSerializer.h"
#include <iostream>
#include <filesystem>

namespace Metroidvania {

    bool LevelSerializer::save(
        const LevelData& data,
        const TileMap& bgLayer,
        const TileMap& mgLayer,
        const TileMap& fgLayer,
        const TileMap& parallaxFront)
    {
        const std::string path = buildPath(data.name);

        // create output directory if it doesn't exist
        std::filesystem::create_directories("assets/levels/levelJSONS");

        JsonWriter writer;

        // level metadata
        writer.beginObject("level");
        writer.writeString("name", data.name);
        writer.writeInt("columns", data.columns);
        writer.writeInt("rows", data.rows);
        writer.endObject();

        // player spawn position
        writer.beginObject("player_spawn");
        writer.writeFloat("x", data.playerSpawn.x);
        writer.writeFloat("y", data.playerSpawn.y);
        writer.endObject();

        // parallax config
        writer.beginObject("parallax");
        writer.writeString("background_texture", data.plxBkgPath);
        writer.writeString("mid_texture", data.plxMidPath);
        writer.writeIntArray("background_color", {
            static_cast<int>(data.bgColor.r),
            static_cast<int>(data.bgColor.g),
            static_cast<int>(data.bgColor.b)
            });
        writer.endObject();

        // tile layers - sparse, only non-empty tiles written
        writer.beginObject("layers");
        serializeLayer(writer, "background", bgLayer);
        serializeLayer(writer, "midground", mgLayer);
        serializeLayer(writer, "foreground", fgLayer);
        serializeLayer(writer, "parallax_front", parallaxFront);
        writer.endObject();

        // entities - stub, populated when entity system is built
        writer.beginArray("entities");
        // Enemy entries go here    - { "type": "enemy",    "x": 0.0, "y": 0.0 }
        // Pickup entries go here   - { "type": "pickup",   "x": 0.0, "y": 0.0 }
        // Chest entries go here    - { "type": "chest",    "x": 0.0, "y": 0.0 }
        // Breakable entries go here- { "type": "breakable","x": 0.0, "y": 0.0 }
        writer.endArray();

        if (!writer.writeToFile(path))
        {
            showError("save", "Could not write to file: " + path);
            return false;
        }

        std::cout << "[LevelSerializer] Saved: " << path << "\n";
        return true;
    }

    bool LevelSerializer::load(
        const std::string& name,
        LevelData& data,
        TileMap& bgLayer,
        TileMap& mgLayer,
        TileMap& fgLayer,
        TileMap& parallaxFront)
    {
        const std::string path = buildPath(name);

        JsonReader reader;
        if (!reader.loadFromFile(path))
        {
            showError("load", reader.getError());
            return false;
        }

        const JsonValue& root = reader.getRoot();

        // read level metadata
        if (!root.hasKey("level"))
        {
            showError("load", "Missing \"level\" object in: " + path);
            return false;
        }

        const JsonValue& level = root["level"];

        if (!level.hasKey("name") || !level.hasKey("columns") || !level.hasKey("rows"))
        {
            showError("load", "Missing key in \"level\" object - expected name, columns, rows");
            return false;
        }

        data.name = level["name"].getString();
        data.columns = level["columns"].getInt();
        data.rows = level["rows"].getInt();

        // read player spawn
        if (!root.hasKey("player_spawn"))
        {
            showError("load", "Missing \"player_spawn\" object in: " + path);
            return false;
        }

        const JsonValue& spawn = root["player_spawn"];
        data.playerSpawn.x = spawn["x"].getFloat();
        data.playerSpawn.y = spawn["y"].getFloat();

        // read parallax config
        if (!root.hasKey("parallax"))
        {
            showError("load", "Missing \"parallax\" object in: " + path);
            return false;
        }

        const JsonValue& plx = root["parallax"];
        data.plxBkgPath = plx["background_texture"].getString();
        data.plxMidPath = plx["mid_texture"].getString();

        if (plx.hasKey("background_color") && plx["background_color"].size() == 3)
        {
            const JsonValue& col = plx["background_color"];
            data.bgColor = sf::Color(
                static_cast<uint8_t>(col[0].getInt()),
                static_cast<uint8_t>(col[1].getInt()),
                static_cast<uint8_t>(col[2].getInt())
            );
        }

        // read tile layers
        if (!root.hasKey("layers"))
        {
            showError("load", "Missing \"layers\" object in: " + path);
            return false;
        }

        const JsonValue& layers = root["layers"];

        if (!deserializeLayer(layers["background"], "background", bgLayer))      return false;
        if (!deserializeLayer(layers["midground"], "midground", mgLayer))      return false;
        if (!deserializeLayer(layers["foreground"], "foreground", fgLayer))      return false;
        if (!deserializeLayer(layers["parallax_front"], "parallax_front", parallaxFront))return false;

        // entities - stub, nothing to read yet
        // when entity system is built, read root["entities"] array here

        std::cout << "[LevelSerializer] Loaded: " << path << "\n";
        return true;
    }

    void LevelSerializer::serializeLayer(
        JsonWriter& writer,
        const std::string& key,
        const TileMap& map)
    {
        writer.beginArray(key);

        for (int row = 0; row < map.getRows(); ++row)
        {
            for (int col = 0; col < map.getColumns(); ++col)
            {
                // get tile bounds to check type - only write non-empty tiles
                const sf::FloatRect bounds(
                    sf::Vector2f(col * k_tileSize, row * k_tileSize),
                    sf::Vector2f(k_tileSize, k_tileSize)
                );

                // query the tile directly via index
                const Tile& tile = map.getTile(col, row);
                if (tile.type == TileType::Empty) continue;

                writer.writeRawIntArray({
                    col,
                    row,
                    tileTypeToInt(tile.type)
                    });
            }
        }

        writer.endArray();
    }

    bool LevelSerializer::deserializeLayer(
        const JsonValue& layerArray,
        const std::string& layerName,
        TileMap& map)
    {
        if (layerArray.isNull())
        {
            showError("load", "Missing layer: \"" + layerName + "\"");
            return false;
        }

        if (!layerArray.isArray())
        {
            showError("load", "Layer \"" + layerName + "\" is not an array");
            return false;
        }

        for (int i = 0; i < layerArray.size(); ++i)
        {
            const JsonValue& entry = layerArray[i];

            if (!entry.isArray() || entry.size() < 3)
            {
                showError("load", "Bad tile entry in layer \"" + layerName
                    + "\" at index " + std::to_string(i));
                return false;
            }

            const int col = entry[0].getInt();
            const int row = entry[1].getInt();
            const int type = entry[2].getInt();

            map.setTile(col, row, intToTileType(type));
        }

        return true;
    }

    int LevelSerializer::tileTypeToInt(TileType type) const
    {
        switch (type)
        {
        case TileType::Stone:    return 1;
        case TileType::Platform: return 2;
        case TileType::Slope:    return 3;
        case TileType::Empty:    return 0;
        default:                 return 0;
        }
    }

    TileType LevelSerializer::intToTileType(int val) const
    {
        switch (val)
        {
        case 1:  return TileType::Stone;
        case 2:  return TileType::Platform;
        case 3:  return TileType::Slope;
        default: return TileType::Empty;
        }
    }

    std::string LevelSerializer::buildPath(const std::string& name) const
    {
        return "assets/levels/levelJSONS/" + name + ".json";
    }

    void LevelSerializer::showError(
        const std::string& context,
        const std::string& reason) const
    {
        const std::string msg =
            "[LevelSerializer] Failed to " + context + "\n"
            "Reason: " + reason;

        std::cerr << msg << "\n";

#ifdef _WIN32
        MessageBoxA(nullptr, msg.c_str(), "LevelSerializer Error", MB_OK | MB_ICONERROR);
#endif
    }

}