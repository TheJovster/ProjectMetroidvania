#include "Player.h"

namespace Metroidvania {

    Player::Player(sf::Vector2f position)
        : Entity(position, sf::Vector2f(64.f, 96.f)) // placeholder size - replace with sprite dims
    {
        // --- Visual ---
        m_shape.setSize(m_size);
        m_shape.setFillColor(sf::Color(180, 60, 60));   // placeholder red
        m_shape.setOutlineColor(sf::Color(220, 100, 100));
        m_shape.setOutlineThickness(1.f);

        // --- Animator setup ---
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

        m_animator.addClip(AnimationState::IdlePassive, makeClip(true, true));
        m_animator.addClip(AnimationState::Walk, makeClip(true, true));
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
        m_animator.addClip(AnimationState::Dead, makeClip(false, false));  // force only

        m_animator.setReturnState(AnimationState::IdlePassive);
        m_animator.setFacingRight(true);
    }

    void Player::update(float dt, Input& input, const TileMap& tileMap)
    {
        handleInput(input);
        applyGravity(dt);
        applyMovement(dt);
        resolveCollision(tileMap);
        updateCoyote();
        updateAnimator(input);
        m_animator.update(dt);
    }

    // -------------------------------------------------------
    // Input
    // -------------------------------------------------------
    void Player::handleInput(Input& input)
    {
        // --- Turn logic ---
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
            return; // locked during turn
        }

        // --- Horizontal movement ---
        const bool movingLeft = input.isHeld(Action::MoveLeft);
        const bool movingRight = input.isHeld(Action::MoveRight);

        if (movingLeft && m_animator.isFacingRight())
        {
            // Wants to go left but facing right - trigger turn
            m_turning = true;
            m_turnTimer = k_turnLockDuration;
            m_pendingFlip = true;
            m_velocity.x = 0.f;
        }
        else if (movingRight && !m_animator.isFacingRight())
        {
            // Wants to go right but facing left - trigger turn
            m_turning = true;
            m_turnTimer = k_turnLockDuration;
            m_pendingFlip = true;
            m_velocity.x = 0.f;
        }
        else if (movingLeft)
        {
            m_velocity.x = -k_playerMoveSpeed;
        }
        else if (movingRight)
        {
            m_velocity.x = k_playerMoveSpeed;
        }
        else
        {
            m_velocity.x = 0.f;
        }

        // --- Crouch ---
        m_crouching = input.isHeld(Action::Crouch) && m_grounded;

        // --- Jump ---
        if (canJump() && input.wasRecentlyPressed(Action::Jump))
        {
            jump();
            input.consumeBufferedAction(Action::Jump); // consume so it doesn't re-fire
        }
    }

    // -------------------------------------------------------
    // Physics
    // -------------------------------------------------------
    void Player::applyGravity(float dt)
    {
        if (!m_grounded)
        {
            m_velocity.y += k_gravity * dt;
            m_velocity.y = std::min(m_velocity.y, k_maxFallSpeed);
        }
    }

    void Player::applyMovement(float dt)
    {
        m_position += m_velocity * dt;
        m_shape.setPosition(m_position);
    }

    // -------------------------------------------------------
    // Collision
    // -------------------------------------------------------
    void Player::resolveCollision(const TileMap& tileMap)
    {
        const bool wasGrounded = m_grounded;
        m_grounded = false;

        sf::FloatRect bounds = getBounds();
        auto solidTiles = tileMap.getSolidBounds(bounds);

        for (const sf::FloatRect& tile : solidTiles)
        {
            sf::FloatRect playerBounds = getBounds();
            auto intersection = playerBounds.findIntersection(tile);
            if (!intersection.has_value()) continue;

            const sf::FloatRect overlap = intersection.value();

            // Resolve on shortest axis
            if (overlap.size.x < overlap.size.y)
            {
                // Horizontal resolution
                if (m_position.x < tile.position.x)
                    m_position.x -= overlap.size.x;
                else
                    m_position.x += overlap.size.x;

                m_velocity.x = 0.f;
            }
            else
            {
                // Vertical resolution
                if (m_position.y < tile.position.y)
                {
                    // Landing on top of tile
                    m_position.y -= overlap.size.y;
                    m_velocity.y = 0.f;
                    m_grounded = true;
                    m_jumping = false;
                }
                else
                {
                    // Hitting ceiling
                    m_position.y += overlap.size.y;
                    m_velocity.y = 0.f;
                }
            }
        }

        m_shape.setPosition(m_position);

        // Coyote - just left the ground
        if (wasGrounded && !m_grounded && !m_jumping)
            m_coyoteFrames = k_coyoteFrames;
    }

    // -------------------------------------------------------
    // Coyote time
    // -------------------------------------------------------
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

    // -------------------------------------------------------
    // Jump
    // -------------------------------------------------------
    bool Player::canJump() const
    {
        return m_grounded || m_coyoteAvailable;
    }

    void Player::jump()
    {
        m_velocity.y = k_playerJumpForce;
        m_jumping = true;
        m_grounded = false;
        m_coyoteFrames = 0;
        m_coyoteAvailable = false;
    }

    // -------------------------------------------------------
    // Animator
    // -------------------------------------------------------
    void Player::updateAnimator(const Input& input)
    {
        const AnimationState current = m_animator.getState();

        // --- Forced states take priority - don't touch these here ---
        if (current == AnimationState::Die ||
            current == AnimationState::Dead ||
            current == AnimationState::Hurt)
            return;

        // --- Turn ---
        if (m_turning)
        {
            m_animator.setState(AnimationState::Turn);
            return;
        }

        // --- Airborne ---
        if (!m_grounded)
        {
            if (m_velocity.y < -100.f)
                m_animator.setState(AnimationState::JumpAscent);
            else if (m_velocity.y < 100.f)
                m_animator.setState(AnimationState::JumpApex);
            else
                m_animator.setState(AnimationState::Fall);
            return;
        }

        // --- Grounded ---
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

        // Land
        if (current == AnimationState::Fall ||
            current == AnimationState::JumpApex ||
            current == AnimationState::JumpAscent)
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

    // -------------------------------------------------------
    // Draw
    // -------------------------------------------------------
    void Player::draw(sf::RenderWindow& window)
    {
        window.draw(m_shape);
    }

}