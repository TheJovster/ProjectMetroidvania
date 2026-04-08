#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "TileMap.h"
#include "PhysicsConstants.h"

namespace Metroidvania {

    //Patrol distances from spawn point
    constexpr float k_patrolLeftDistance = 200.f;
    constexpr float k_patrolRightDistance = 200.f;

    //Movement speed
    constexpr float k_enemyMoveSpeed = 80.f;

    //Aggro / deaggro ranges
    constexpr float k_enemyDetectRange = 300.f;  // horizontal distance to aggro
    constexpr float k_enemyDeaggroRange = 500.f;  // horizontal distance to deaggro

    //Health
    constexpr int k_enemyMaxHealth = 3;

    //Hit flash duration
    constexpr float k_enemyHitFlashDuration = 0.1f;

    class Enemy : public Entity {
    public:
        explicit Enemy(sf::Vector2f position);

        void update(float dt, const TileMap& tileMap, sf::Vector2f playerPosition);
        void draw(sf::RenderWindow& window) override;

        void takeDamage(int amount);
        bool isDead() const { return m_health <= 0; }
        int  getHealth() const { return m_health; }

    private:
        //Health
        int   m_health = k_enemyMaxHealth;

        //Hit flash 
        bool  m_flashing = false;
        float m_flashTimer = 0.f;

        //patrol points
        sf::Vector2f m_patrolLeft;
        sf::Vector2f m_patrolRight;
        sf::Vector2f m_currentTarget;

        //AI state
        enum class AIState {
            Patrol,
            Aggro
        };

        AIState      m_aiState = AIState::Patrol;

        //Visual
        sf::RectangleShape m_shape;

        //Internal
        void updatePatrol(float dt, const TileMap& tileMap);
        void updateAggro(float dt, const TileMap& tileMap, sf::Vector2f playerPosition);
        void updateAI(sf::Vector2f playerPosition);
        void moveToward(sf::Vector2f target, float dt);
        bool detectPlayer(sf::Vector2f playerPosition) const;
        bool deaggroPlayer(sf::Vector2f playerPosition) const;
    };

}