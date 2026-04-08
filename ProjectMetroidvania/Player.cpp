#include "Player.h"

namespace Metroidvania {

    Player::Player(sf::Vector2f position, TextureCache& textureCache)
        : Entity(position, sf::Vector2f(35.f, 80.f)) // placeholder size - replace with sprite dims
        , m_textureCache(textureCache)
        , m_sprite(textureCache.get("assets/animations/characters/gideon/idle/Gideon_idle_01.png"))
    {
        // Visual
        m_physicsBody.getPosition().x + m_physicsBody.getSize().x * 0.5f,
        m_physicsBody.getPosition().y + m_physicsBody.getSize().y * 0.5f + k_colliderBottomOffset;

        // Animator setup
        // Placeholder clips - single frame each until assets are in
        // Loop = true for sustained states, false for one-shots

        auto makeClip = [](bool loop, bool interruptible, int interruptFrame = 0) {
            AnimationClip clip;
            clip.frames = { "" };   // empty frame - no texture yet
            clip.loop = loop;
            clip.interruptible = interruptible;
            clip.interruptFrame = interruptFrame;
            clip.fps = 12.f;
            return clip;
            };

        m_animator.addClip(AnimationState::IdlePassive,
            buildClip("assets/animations/characters/gideon/idle/",
                "Gideon_idle_", 8, true, 6.f));

        m_animator.addClip(AnimationState::Walk,
            buildClip("assets/animations/characters/gideon/walk/",
                "Gideon_Walk_", 9, true, 6.f));

        m_animator.addClip(AnimationState::JumpAscent,
            buildClip("assets/animations/characters/gideon/jump/ascent/",
                "Gideon_Ascent_", 3, false, 6.f));

        m_animator.addClip(AnimationState::JumpApex,
            buildClip("assets/animations/characters/gideon/jump/apex/",
                "Gideon_Apex_", 2, true, 6.f));

        m_animator.addClip(AnimationState::Fall,
            buildClip("assets/animations/characters/gideon/jump/fall/",
                "Gideon_Fall_", 3, true, 6.f));

        m_animator.addClip(AnimationState::Land,
            buildClip("assets/animations/characters/gideon/jump/land/",
                "Gideon_Land_", 2, false, 12.f, false));
        
        m_animator.addClip(AnimationState::AttackPrimary,
            buildClip("assets/animations/characters/gideon/attack/",
                "Gideon_Attack_", 8, false, 12.0f, false));

        m_animator.addClip(AnimationState::AttackAir,
            buildClip("assets/animations/characters/gideon/airattack/",
            "Gideon_AirAttack_", 8, false, 12.0f, false));

/*        m_animator.addClip(AnimationState::Walk, makeClip(true, true));
        m_animator.addClip(AnimationState::Run, makeClip(true, true));
        m_animator.addClip(AnimationState::Turn, makeClip(false, false));  // must complete
        m_animator.addClip(AnimationState::JumpAscent, makeClip(false, true));
        m_animator.addClip(AnimationState::JumpApex, makeClip(true, true));
        m_animator.addClip(AnimationState::Fall, makeClip(true, true));
        m_animator.addClip(AnimationState::Land, makeClip(false, false));  // must complete
        m_animator.addClip(AnimationState::Crouch, makeClip(false, false));
        m_animator.addClip(AnimationState::CrouchIdle, makeClip(true, true));
        m_animator.addClip(AnimationState::CrouchRise, makeClip(false, false));
        m_animator.addClip(AnimationState::Hurt, makeClip(false, false));  // force only
        m_animator.addClip(AnimationState::Die, makeClip(false, false));  // force only
        m_animator.addClip(AnimationState::Dead, makeClip(false, false));  // force only*/

        m_sprite.setOrigin(sf::Vector2f(48.f, 48.f));

        m_animator.setReturnState(AnimationState::IdlePassive);
        m_animator.setFacingRight(true);
    }

    void Player::update(float dt, Input& input, const TileMap& tileMap)
    {
        m_wasGrounded = m_physicsBody.isGrounded();

        //physics - gravity + collision + state flags
        m_physicsBody.update(dt, tileMap);

        //variable height jump - scale gravity while holding and ascending
        const bool floating = m_jumpHeld
            && m_jumpState == JumpState::FirstJump
            && m_physicsBody.getVelocity().y < k_jumpHoldMaxVelocity;

        m_physicsBody.setGravityScale(floating ? k_jumpHoldGravityScale : 1.f);

        //react to landing
        if (m_physicsBody.hasLanded())
        {
            m_jumpState = JumpState::Grounded;
            m_hasDoubleJump = true;
        }

        //coyote - only when walked off edge, not jumped off
        if (!m_physicsBody.isGrounded() &&
            m_wasGrounded &&
            m_jumpState == JumpState::Grounded)
        {
            m_coyoteFrames = k_coyoteFrames;
            m_jumpState = JumpState::FirstJump;  // prevent re-triggering
        }

        //read input and inject forces
        handleInput(input);

        updateCoyote();
        updateAnimator(input);
        m_animator.update(dt);

        if (m_animator.justCompleted())
        {
            const AnimationState completed = m_animator.getLastCompletedState();
            if (completed == AnimationState::AttackPrimary ||
                completed == AnimationState::AttackAir)
            {
                m_combatComponent.setIsAttacking(false);
                m_combatComponent.setCanAttack(true);
            }
        }
    }

    void Player::handleInput(Input& input)
    {
        m_jumpHeld = input.isHeld(Action::Jump);

        //turn logic
        if (m_turning)
        {
            m_turnTimer -= 1.f / 60.f;
            if (m_turnTimer <= 0.f)
            {
                m_turning = false;
                if (m_pendingFlip)
                {
                    m_animator.setFacingRight(!m_animator.isFacingRight());
                    m_pendingFlip = false;
                }
            }
            return;
        }

        //horizontal movement
        const bool movingLeft = input.isHeld(Action::MoveLeft);
        const bool movingRight = input.isHeld(Action::MoveRight);

        if (movingLeft && m_animator.isFacingRight())
        {
            m_turning = true;
            m_turnTimer = k_turnLockDuration;
            m_pendingFlip = true;
            m_physicsBody.setVelocity({ 0.f, m_physicsBody.getVelocity().y });
        }
        else if (movingRight && !m_animator.isFacingRight())
        {
            m_turning = true;
            m_turnTimer = k_turnLockDuration;
            m_pendingFlip = true;
            m_physicsBody.setVelocity({ 0.f, m_physicsBody.getVelocity().y });
        }
        else if (movingLeft)
            m_physicsBody.setVelocity({ -k_playerMoveSpeed, m_physicsBody.getVelocity().y });
        else if (movingRight)
            m_physicsBody.setVelocity({ k_playerMoveSpeed, m_physicsBody.getVelocity().y });
        else
            m_physicsBody.setVelocity({ 0.f, m_physicsBody.getVelocity().y });

        //crouch
        m_crouching = input.isHeld(Action::Crouch) && m_physicsBody.isGrounded();

        //jump injection
        if (canJump() && input.wasRecentlyPressed(Action::Jump))
        {
            jump();
            input.consumeBufferedAction(Action::Jump);
        }

        //attack
        if (input.isJustPressed(Action::AttackPrimary) && m_combatComponent.getCanAttack())
        {
            m_combatComponent.setIsAttacking(true);
            m_combatComponent.setCanAttack(false);
            m_hasHit = false;
        }
    }

    void Player::updateCoyote()
    {
        if (m_coyoteFrames > 0)
        {
            m_coyoteFrames--;
            m_coyoteAvailable = true;
        }
        else
        {
            m_coyoteAvailable = false;
        }
    }

    bool Player::canJump() const
    {
        // grounded or coyote - first jump available
        if (m_physicsBody.isGrounded() || m_coyoteAvailable)
            return true;

        // double jump - only from FirstJump, never from SecondJump
        if (m_jumpState == JumpState::FirstJump &&
            m_hasDoubleJump &&
            m_abilitySet.hasUnlocked(AbilitySlot::DoubleJump))
            return true;

        return false;
    }

    void Player::jump()
    {
        const bool isDoubleJump = !m_physicsBody.isGrounded() && !m_coyoteAvailable;

        if (isDoubleJump)
        {
            m_jumpState = JumpState::SecondJump;
            m_hasDoubleJump = false;
            m_jumpHeld = false;
        }
        else
        {
            m_jumpState = JumpState::FirstJump;
            m_jumpHeld = false;
        }

        m_physicsBody.jump(k_playerJumpForce);
        m_coyoteFrames = 0;
        m_coyoteAvailable = false;
    }

    void Player::updateAnimator(const Input& input)
    {
        const AnimationState current = m_animator.getState();

        //forced states take priority
        if (current == AnimationState::Die ||
            current == AnimationState::Dead ||
            current == AnimationState::Hurt)
            return;

        // Turn
        if (m_turning)
        {
            m_animator.setState(AnimationState::Turn);
            return;
        }

        // Attack
        if (input.isJustPressed(Action::AttackPrimary))
        {
            m_hasHit = false;
            if (m_physicsBody.isGrounded())        // was m_grounded
                m_animator.setState(AnimationState::AttackPrimary);
            else
                m_animator.setState(AnimationState::AttackAir);
            return;
        }

        // if attack is playing - let it finish
        if ((current == AnimationState::AttackPrimary ||
            current == AnimationState::AttackAir) && !m_animator.isComplete())
            return;

        //Airborne states
        if (!m_physicsBody.isGrounded())           // was !m_grounded
        {
            if (m_physicsBody.getVelocity().y < -100.f)
                m_animator.setState(AnimationState::JumpAscent);
            else if (m_physicsBody.getVelocity().y < 100.f)
                m_animator.setState(AnimationState::JumpApex);
            else
                m_animator.setState(AnimationState::Fall);
            return;
        }

        m_crouching = input.isHeld(Action::Crouch) && m_physicsBody.isGrounded();

        // Land trigger
        if (m_physicsBody.isGrounded() && !m_wasGrounded)  // was m_grounded && !m_wasGrounded
        {
            m_animator.setState(AnimationState::Land);
            return;
        }

        // Grounded
        if (m_crouching)
        {
            if (current != AnimationState::CrouchIdle &&
                current != AnimationState::Crouch)
                m_animator.setState(AnimationState::Crouch);
            else if (m_animator.isComplete())
                m_animator.setState(AnimationState::CrouchIdle);
            return;
        }

        // Coming out of crouch
        if (current == AnimationState::CrouchIdle ||
            current == AnimationState::Crouch)
        {
            m_animator.setState(AnimationState::CrouchRise);
            return;
        }

        // if landing animation is playing - let it finish
        if (current == AnimationState::Land && !m_animator.isComplete())
            return;

        // only trigger land on the frame we touch ground
        if (m_physicsBody.isGrounded() && !m_wasGrounded)
        {
            m_animator.setState(AnimationState::Land);
            return;
        }

        // Walk / Idle
        const bool moving = input.isHeld(Action::MoveLeft) ||
            input.isHeld(Action::MoveRight);

        if (moving)
            m_animator.setState(AnimationState::Walk);
        else
            m_animator.setState(AnimationState::IdlePassive);
    }

    // Draw
    void Player::draw(sf::RenderWindow& window)
    {
        // get current frame path from animator
        const std::string& framePath = m_animator.getCurrentFrame();
        if (!framePath.empty())
        {
            // look up texture in cache
            const sf::Texture& texture = m_textureCache.get(framePath);
            m_sprite.setTexture(texture);

            // flip on X based on facing direction
            const float scaleX = m_animator.isFacingRight() ? 1.f : -1.f;
            m_sprite.setScale(sf::Vector2f(scaleX, 1.f));

            // position sprite - origin is centre so offset by half size
            m_sprite.setPosition(sf::Vector2f(
                m_physicsBody.getPosition().x + m_physicsBody.getSize().x * 0.5f,
                m_physicsBody.getPosition().y + m_physicsBody.getSize().y * 0.5f
            ));
            window.draw(m_sprite);
        }
        else
        {
            // DEBUG - draw red rectangle if no frame available
            window.draw(m_shape);
        }
    }

    std::optional<sf::FloatRect> Player::getAttackHitbox() const
    {
        const AnimationState current = m_animator.getState();

        // only active during attack states
        if (current != AnimationState::AttackPrimary &&
            current != AnimationState::AttackAir)
            return std::nullopt;

        // only active during frames 3-6 (index 2-5)
        const int frame = m_animator.getCurrentFrameIndex();
        if (frame < 2 || frame > 5)
            return std::nullopt;

        // position hitbox just outside Gideon in facing direction
        const float x = m_animator.isFacingRight()
            ? m_physicsBody.getPosition().x + (m_physicsBody.getSize().x * 0.5f) + k_attackHitboxWidth
            : m_physicsBody.getPosition().x - (m_physicsBody.getSize().x * 0.5f) - k_attackHitboxWidth;

        return sf::FloatRect(
            sf::Vector2f(x, m_physicsBody.getPosition().y),
            sf::Vector2f(k_attackHitboxWidth, k_attackHitboxHeight)
        );
    }

}