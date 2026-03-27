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

        m_shape.setSize(m_size);
        m_shape.setFillColor(sf::Color(180, 60, 60));
        m_shape.setOutlineColor(sf::Color(220, 100, 100));
        m_shape.setOutlineThickness(1.f);
    }

    void Enemy::update(float dt, const TileMap& tileMap, sf::Vector2f playerPosition)
    {
        if (!m_alive) return;

        // tick hit flash
        if (m_flashing)
        {
            m_flashTimer -= dt;
            if (m_flashTimer <= 0.f)
            {
                m_flashing = false;
                m_shape.setFillColor(sf::Color(180, 60, 60));
            }
        }

        updateAI(playerPosition);

        // stop moving if overlapping player
        sf::FloatRect enemyBounds = getBounds();
        sf::FloatRect playerBounds(playerPosition, sf::Vector2f(40.f, 80.f));

        if (enemyBounds.findIntersection(playerBounds).has_value())
            m_velocity.x = 0.f;
        else
        {
            if (m_aiState == AIState::Patrol)
                updatePatrol(dt, tileMap);
            else
                updateAggro(dt, tileMap, playerPosition);
        }

        resolveCollision(tileMap);
        m_shape.setPosition(m_position);
    }

    void Enemy::draw(sf::RenderWindow& window)
    {
        if (!m_alive) return;
        window.draw(m_shape);
    }

    void Enemy::takeDamage(int amount)
    {
        if (!m_alive) return;

        m_health = std::max(0, m_health - amount);

        if (m_health <= 0)
        {
            kill();
            return;
        }

        // hit flash
        m_flashing = true;
        m_flashTimer = k_enemyHitFlashDuration;
        m_shape.setFillColor(sf::Color::White);
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
        const float distToTarget = std::abs(m_position.x - m_currentTarget.x);
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
        sf::Vector2f chaseTarget(playerPosition.x, m_position.y);
        moveToward(chaseTarget, dt);
    }

    void Enemy::moveToward(sf::Vector2f target, float dt)
    {
        const float dx = target.x - m_position.x;
        const float dir = (dx > 0.f) ? 1.f : -1.f;

        m_velocity.x = dir * k_enemyMoveSpeed;
        m_position += m_velocity * dt;
    }

    void Enemy::resolveCollision(const TileMap& tileMap)
    {
        sf::FloatRect bounds = getBounds();
        auto solidTiles = tileMap.getSolidBounds(bounds);

        for (const sf::FloatRect& tile : solidTiles)
        {
            sf::FloatRect enemyBounds = getBounds();
            auto intersection = enemyBounds.findIntersection(tile);
            if (!intersection.has_value()) continue;

            const sf::FloatRect overlap = intersection.value();

            if (overlap.size.x < overlap.size.y)
            {
                // wall hit - reverse patrol target
                if (m_position.x < tile.position.x)
                    m_position.x -= overlap.size.x;
                else
                    m_position.x += overlap.size.x;

                m_velocity.x = 0.f;

                // reverse patrol direction on wall hit
                if (m_aiState == AIState::Patrol)
                {
                    m_currentTarget = (m_currentTarget.x == m_patrolRight.x)
                        ? m_patrolLeft
                        : m_patrolRight;
                }
            }
            else
            {
                // vertical resolution
                if (m_position.y < tile.position.y)
                {
                    m_position.y -= overlap.size.y;
                    m_velocity.y = 0.f;
                }
                else
                {
                    m_position.y += overlap.size.y;
                    m_velocity.y = 0.f;
                }
            }
        }

        // apply gravity
        m_velocity.y += k_gravity * (1.f / 60.f);
        m_velocity.y = std::min(m_velocity.y, k_maxFallSpeed);
    }

    bool Enemy::detectPlayer(sf::Vector2f playerPosition) const
    {
        const float dx = std::abs(playerPosition.x - m_position.x);
        const float dy = std::abs(playerPosition.y - m_position.y);
        return dx <= k_enemyDetectRange && dy <= k_tileSize;
    }

    bool Enemy::deaggroPlayer(sf::Vector2f playerPosition) const
    {
        const float dx = std::abs(playerPosition.x - m_position.x);
        return dx > k_enemyDeaggroRange;
    }

}