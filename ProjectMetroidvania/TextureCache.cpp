#include "TextureCache.h"
#include <iostream>

namespace Metroidvania {

    const sf::Texture& TextureCache::get(const std::string& path)
    {
        // return cached texture if already loaded
        auto it = m_cache.find(path);
        if (it != m_cache.end())
            return *it->second;

        // try to load from disk
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(path))
        {
            // DEBUG - log missing texture and return red placeholder
            std::cerr << "[TextureCache] Failed to load: " << path << "\n";
            return getFallback();
        }

        // cache and return
        const sf::Texture& ref = *texture;
        m_cache[path] = std::move(texture);
        return ref;
    }

    bool TextureCache::has(const std::string& path) const
    {
        return m_cache.find(path) != m_cache.end();
    }

    void TextureCache::clear()
    {
        m_cache.clear();
        std::cout << "[TextureCache] Cache cleared\n";
    }

    const sf::Texture& TextureCache::getFallback()
    {
        // DEBUG - builds a 96x96 red placeholder texture once
        if (!m_fallbackReady)
        {
            sf::Image img({ 96, 96 }, sf::Color(180, 60, 60));
            m_fallback.loadFromImage(img);
            m_fallbackReady = true;
        }
        return m_fallback;
    }

}