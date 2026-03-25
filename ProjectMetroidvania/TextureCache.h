#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace Metroidvania {

    class TextureCache {
    public:
        TextureCache() = default;

        // loads a texture from path and caches it - returns cached version if already loaded
        const sf::Texture& get(const std::string& path);

        // returns true if the texture is already in the cache
        bool has(const std::string& path) const;

        // clears all cached textures - call on level transition
        void clear();

    private:
        std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_cache;

        // DEBUG - logged when a texture fails to load, falls back to placeholder
        const sf::Texture& getFallback();
        sf::Texture m_fallback;
        bool        m_fallbackReady = false;
    };

}