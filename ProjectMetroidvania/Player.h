#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include "Entity.h"
#include "Input.h"
#include "Animator.h"
#include "AbilitySet.h"
#include "TileMap.h"
#include "TextureCache.h"
#include "AnimationHelper.h"
#include "DoubleJumpAbility.h"
#include "PhysicsConstants.h"


namespace Metroidvania {

    constexpr float k_attackHitboxWidth = 30.f;
    constexpr float k_attackHitboxHeight = 80.f;

    constexpr float k_playerMoveSpeed = 300.f;
    constexpr float k_playerJumpForce = -500.f;

    constexpr int   k_coyoteFrames = 6;       // tight - tune during feel pass
    constexpr float k_turnLockDuration = 0.1f;    // seconds the turn animation locks movement
    constexpr float k_jumpHoldGravityScale = 0.45f;  // gravity multiplier while holding jump - tune during feel pass
    constexpr float k_jumpHoldMaxVelocity = -200.f; // stop boosting above this velocity - tune during feel pass

    class Player : public Entity {
    public:
        explicit Player(sf::Vector2f position, TextureCache& textureCache);


        void update(float dt, Input& input, const TileMap& tileMap);
        void draw(sf::RenderWindow& window) override;

        bool isGrounded()    const { return m_grounded; }
        bool isJumping()     const { return m_jumping; }
        bool isTurning()     const { return m_turning; }
        bool isCrouching()   const { return m_crouching; }

        const Animator& getAnimator() const { return m_animator; }

        AbilitySet& getAbilitySet() { return m_abilitySet; }

        std::optional<sf::FloatRect> getAttackHitbox() const;

        bool  hasHit()    const { return m_hasHit; }
        void  setHasHit(bool val) { m_hasHit = val; }

    private:
        Animator m_animator;
        AbilitySet m_abilitySet;

        bool  m_grounded = false;
        bool m_wasGrounded = false;
        bool  m_jumping = false;
        bool  m_crouching = false;

        int   m_coyoteFrames = 0;     // counts down from k_coyoteFrames
        bool  m_coyoteAvailable = false;

        bool  m_turning = false;
        float m_turnTimer = 0.f;
        bool  m_pendingFlip = false; // flip sprite when turn animation completes

        bool m_hasHit = false;

        //abilities - later on dependant on Ability Set but exposed here for testing
        bool m_hasDoubleJump = false;

        TextureCache& m_textureCache;
        sf::Sprite    m_sprite;

        void handleInput(Input& input);
        void updateAnimator(const Input& input);  
        void applyGravity(float dt);
        void applyMovement(float dt);
        void resolveCollision(const TileMap& tileMap);
        void updateCoyote();

        void jump();
        bool canJump() const;
        bool m_jumpHeld;

        sf::RectangleShape m_shape;


    };

}