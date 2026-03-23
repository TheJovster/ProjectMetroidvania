#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Input.h"
#include "Animator.h"
#include "AbilitySet.h"
#include "TileMap.h"

namespace Metroidvania {

    // --- Tuning constants ---
    constexpr float k_playerMoveSpeed = 300.f;
    constexpr float k_playerJumpForce = -700.f;
    constexpr float k_gravity = 1800.f;
    constexpr float k_maxFallSpeed = 900.f;
    constexpr int   k_coyoteFrames = 6;       // tight - tune during feel pass
    constexpr float k_turnLockDuration = 0.1f;    // seconds the turn animation locks movement

    class Player : public Entity {
    public:
        explicit Player(sf::Vector2f position);

        void update(float dt, Input& input, const TileMap& tileMap);
        void draw(sf::RenderWindow& window) override;

        // --- State queries ---
        bool isGrounded()    const { return m_grounded; }
        bool isJumping()     const { return m_jumping; }
        bool isTurning()     const { return m_turning; }
        bool isCrouching()   const { return m_crouching; }

        // --- Ability access ---
        AbilitySet& getAbilitySet() { return m_abilitySet; }

    private:
        // --- Sub components ---
        Animator   m_animator;
        AbilitySet m_abilitySet;

        // --- Physics state ---
        bool  m_grounded = false;
        bool  m_jumping = false;
        bool  m_crouching = false;

        // --- Coyote time ---
        int   m_coyoteFrames = 0;     // counts down from k_coyoteFrames
        bool  m_coyoteAvailable = false;

        // --- Turn state ---
        bool  m_turning = false;
        float m_turnTimer = 0.f;
        bool  m_pendingFlip = false; // flip sprite when turn animation completes

        // --- Systems ---
        void handleInput(Input& input);
        void updateAnimator(const Input& input);  
        void applyGravity(float dt);
        void applyMovement(float dt);
        void resolveCollision(const TileMap& tileMap);
        void updateCoyote();

        // --- Helpers ---
        void jump();
        bool canJump() const;

        // --- Visual ---
        sf::RectangleShape m_shape;
    };

}