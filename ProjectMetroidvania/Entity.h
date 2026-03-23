#pragma once
#include <SFML/Graphics.hpp>

namespace Metroidvania {

    class Entity {
    public:
        explicit Entity(sf::Vector2f position, sf::Vector2f size);
        virtual ~Entity() = default;

        virtual void update(float dt) {}
        virtual void draw(sf::RenderWindow& window) = 0;

        // --- Collision ---
        sf::FloatRect getBounds() const;
        bool intersects(const Entity& other) const;

        // --- Getters/Setters ---
        sf::Vector2f getPosition()  const { return m_position; }
        sf::Vector2f getVelocity()  const { return m_velocity; }
        sf::Vector2f getSize()      const { return m_size; }
        bool         isAlive()      const { return m_alive; }

        void setPosition(sf::Vector2f position) { m_position = position; }
        void setVelocity(sf::Vector2f velocity) { m_velocity = velocity; }
        void kill() { m_alive = false; }

    protected:
        sf::Vector2f m_position;
        sf::Vector2f m_velocity;
        sf::Vector2f m_size;
        bool         m_alive = true;
    };

}