#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsBody.h"

namespace Metroidvania {

    class Entity {
    public:
        explicit Entity(sf::Vector2f position, sf::Vector2f size);
        virtual ~Entity() = default;

        virtual void update(float dt) {}
        virtual void draw(sf::RenderWindow& window) = 0;

        //physics delegation
        sf::Vector2f  getPosition()  const { return m_physicsBody.getPosition(); }
        sf::Vector2f  getVelocity()  const { return m_physicsBody.getVelocity(); }
        sf::Vector2f  getSize()      const { return m_physicsBody.getSize(); }
        sf::FloatRect getBounds()    const { return m_physicsBody.getBounds(); }
        bool          isAlive()      const { return m_alive; }

        void setPosition(sf::Vector2f pos) { m_physicsBody.setPosition(pos); }
        void setVelocity(sf::Vector2f vel) { m_physicsBody.setVelocity(vel); }
        void kill() { m_alive = false; }

    protected:
        PhysicsBody m_physicsBody;
        bool        m_alive = true;
    };

}