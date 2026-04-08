#include "PhysicsBody.h"

namespace Metroidvania {

    PhysicsBody::PhysicsBody(sf::Vector2f position, sf::Vector2f size)
        : m_position(position)
        , m_size(size)
        , m_velocity(0.f, 0.f)
    {
    }

    void PhysicsBody::update(float dt, const TileMap& tileMap)
    {
        m_hasLanded = false;
        m_hitWall = false;
        m_wasGrounded = m_grounded;

        applyGravity(dt);
        m_position += m_velocity * dt;  // move before resolving
        resolveCollision(tileMap);

        if (m_grounded && !m_wasGrounded)
            m_hasLanded = true;
    }

    void PhysicsBody::applyMovement(float dt)
    {
        m_position += m_velocity * dt;
    }

    sf::FloatRect PhysicsBody::getBounds() const
    {
        return sf::FloatRect(m_position, m_size);
    }

    void PhysicsBody::jump(float jumpForce)
    {
        m_velocity.y = jumpForce;
        m_grounded = false;
    }

    void PhysicsBody::applyGravity(float dt)
    {
        if (!m_grounded)
        {
            m_velocity.y += k_gravity * m_gravityScale * dt;
            m_velocity.y = std::min(m_velocity.y, k_maxFallSpeed);
        }
        else
        {
            //press into floor to keep grounding stable
            m_velocity.y = 0.f;
        }
    }

    void PhysicsBody::resolveCollision(const TileMap& tileMap)
    {
        m_grounded = false;

        //X pass
        sf::FloatRect bounds = getBounds();
        auto solidTiles = tileMap.getSolidBounds(bounds);

        for (const sf::FloatRect& tile : solidTiles)
        {
            auto intersection = getBounds().findIntersection(tile);
            if (!intersection.has_value()) continue;

            const sf::FloatRect overlap = intersection.value();

            if (overlap.size.x <= overlap.size.y)
            {
                if (m_position.x < tile.position.x)
                    m_position.x -= overlap.size.x;
                else
                    m_position.x += overlap.size.x;

                m_velocity.x = 0.f;
                m_hitWall = true;
            }
        }

        //Y pass
        bounds = getBounds();
        solidTiles = tileMap.getSolidBounds(bounds);

        for (const sf::FloatRect& tile : solidTiles)
        {
            auto intersection = getBounds().findIntersection(tile);
            if (!intersection.has_value()) continue;

            const sf::FloatRect overlap = intersection.value();

            if (overlap.size.y <= overlap.size.x)
            {
                if (m_position.y < tile.position.y)
                {
                    m_position.y = tile.position.y - m_size.y;
                    m_velocity.y = 0.f;
                    m_grounded = true;
                }
                else
                {
                    m_position.y += overlap.size.y;
                    m_velocity.y = 0.f;
                }
            }
        }

        //grounding probe - skin width to prevent flicker
        if (!m_grounded)
        {
            sf::FloatRect probeRect(
                sf::Vector2f(m_position.x, m_position.y + 2.f),
                m_size
            );
            auto probeTiles = tileMap.getSolidBounds(probeRect);
            for (const sf::FloatRect& tile : probeTiles)
            {
                if (probeRect.findIntersection(tile).has_value())
                {
                    m_grounded = true;
                    break;
                }
            }
        }
    }

    void PhysicsBody::drawDebug(sf::RenderWindow& window) const
    {
#ifdef _DEBUG
        sf::RectangleShape debugBox;
        debugBox.setSize(m_size);
        debugBox.setPosition(m_position);
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Green);
        debugBox.setOutlineThickness(1.f);
        window.draw(debugBox);
#endif
    }

}