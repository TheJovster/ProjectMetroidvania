#include "Enemy.h"
#include <cmath>
#include <algorithm>

namespace Metroidvania {

    Enemy::Enemy(sf::Vector2f position)
        : Entity(position, sf::Vector2f(40.f, 80.f))
    {
        // patrol points derived from spawn position
        m_patrolLeft = sf::Vector2f(position.x - k_patrolLeftDistance, position.y);
        m_patrolRight = sf::Vector2f(position.x + k_patrolRightDistance, position.y);
        m_currentTarget = m_patrolRight;

        //development visuals 
        m_shape.setSize(sf::Vector2f(m_physicsBody.getSize()));
        m_shape.setFillColor(sf::Color(180, 60, 60));
        m_shape.setOutlineColor(sf::Color(220, 100, 100));
        m_shape.setOutlineThickness(1.f);
    }

    void Enemy::update(float dt, const TileMap& tileMap, sf::Vector2f playerPosition)
    {
        if (!m_alive) return;

        //tick hit flash
        if (m_flashing)
        {
            m_flashTimer -= dt;
            if (m_flashTimer <= 0.f)
            {
                m_flashing = false;
                m_shape.setFillColor(sf::Color(180, 60, 60));  // reset to red
            }
        }

        //physics - gravity + collision + state flags
        m_physicsBody.update(dt, tileMap);
		m_shape.setPosition(m_physicsBody.getPosition());

        //reverse patrol on wall hit
        if (m_physicsBody.hitWall() && m_aiState == AIState::Patrol)
        {
            m_currentTarget = (m_currentTarget.x == m_patrolRight.x)
                ? m_patrolLeft
                : m_patrolRight;
        }

        updateAI(playerPosition);

        //stop moving if overlapping player
        sf::FloatRect playerBounds(playerPosition, sf::Vector2f(40.f, 80.f));
        if (m_physicsBody.getBounds().findIntersection(playerBounds).has_value())
            m_physicsBody.setVelocity(sf::Vector2f(0.f, m_physicsBody.getVelocity().y));
        else
        {
            if (m_aiState == AIState::Patrol)
                updatePatrol(dt, tileMap);
            else
                updateAggro(dt, tileMap, playerPosition);
        }
    }

    void Enemy::draw(sf::RenderWindow& window)
    {
        if (!m_alive) return;
        window.draw(m_shape);

#ifdef _DEBUG
        m_physicsBody.drawDebug(window);
#endif
    }

    void Enemy::takeDamage(int amount)
    {
        if (!m_alive) return;
        m_shape.setFillColor(sf::Color::White);

        m_health = std::max(0, m_health - amount);

        if (m_health <= 0)
        {
            kill();
            return;
        }

        // hit flash
        m_flashing = true;
        m_flashTimer = k_enemyHitFlashDuration;
    }

    void Enemy::updateAI(sf::Vector2f playerPosition)
    {
        if (m_aiState == AIState::Patrol && detectPlayer(playerPosition))
            m_aiState = AIState::Aggro;
        else if (m_aiState == AIState::Aggro && deaggroPlayer(playerPosition))
            m_aiState = AIState::Patrol;
    }

    void Enemy::updatePatrol(float dt, const TileMap& tileMap)
    {
        moveToward(m_currentTarget, dt);

        // reached target - swap to other patrol point
        const float distToTarget = std::abs(m_physicsBody.getPosition().x - m_currentTarget.x);
        if (distToTarget < 4.f)
        {
            m_currentTarget = (m_currentTarget.x == m_patrolLeft.x)
                ? m_patrolRight
                : m_patrolLeft;
        }
    }

    void Enemy::updateAggro(float dt, const TileMap& tileMap, sf::Vector2f playerPosition)
    {
        // chase player horizontally only
        sf::Vector2f chaseTarget(playerPosition.x, m_physicsBody.getPosition().y);
        moveToward(chaseTarget, dt);
    }

    void Enemy::moveToward(sf::Vector2f target, float dt)
    {
        const float dx = target.x - m_physicsBody.getPosition().x;
        const float dir = (dx > 0.f) ? 1.f : -1.f;

        m_physicsBody.setVelocity(sf::Vector2f(dir * k_enemyMoveSpeed, 0.0f));
    }

    bool Enemy::detectPlayer(sf::Vector2f playerPosition) const
    {
        const float dx = std::abs(playerPosition.x - m_physicsBody.getPosition().x);
        const float dy = std::abs(playerPosition.y - m_physicsBody.getPosition().y);
        return dx <= k_enemyDetectRange && dy <= k_tileSize;
    }

    bool Enemy::deaggroPlayer(sf::Vector2f playerPosition) const
    {
        const float dx = std::abs(playerPosition.x - m_physicsBody.getPosition().x);
        return dx > k_enemyDeaggroRange;
    }

}