#pragma once
#include <SFML/Graphics.hpp>
#include "TileMap.h"
#include "PhysicsConstants.h"

namespace Metroidvania {

    class PhysicsBody {
    public:
        explicit PhysicsBody(sf::Vector2f position, sf::Vector2f size);

        //update - gravity + collision + state flags
        void update(float dt, const TileMap& tileMap);

        //movement - apply velocity to position after injection
        void applyMovement(float dt);
        void jump(float jumpForce);


        //state flags
        bool isGrounded() const { return m_grounded; }
        bool hasLanded()  const { return m_hasLanded; }
        bool hitWall()    const { return m_hitWall; }

        //getters
        sf::Vector2f getPosition() const { return m_position; }
        sf::Vector2f getVelocity() const { return m_velocity; }
        sf::Vector2f getSize()     const { return m_size; }
        sf::FloatRect getBounds()  const;

        //setters
        void setPosition(sf::Vector2f position) { m_position = position; }
        void setVelocity(sf::Vector2f velocity) { m_velocity = velocity; }
        void setGravityScale(float scale) { m_gravityScale = scale; }


        //debug
        void drawDebug(sf::RenderWindow& window) const;

    private:
        sf::Vector2f m_position;
        sf::Vector2f m_velocity;
        sf::Vector2f m_size;

        bool m_grounded = false;
        bool m_wasGrounded = false;
        bool m_hasLanded = false;  //true one frame on touchdown
        bool m_hitWall = false;  //true one frame on horizontal collision

        void applyGravity(float dt);
        void resolveCollision(const TileMap& tileMap);

		float m_gravityScale = 1.f;  // multiplier for gravity - allows for variable jump height when holding jump 
    };

}