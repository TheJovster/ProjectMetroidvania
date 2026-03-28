#include "Player.h"

namespace Metroidvania {

    Player::Player(sf::Vector2f position, TextureCache& textureCache)
        : Entity(position, sf::Vector2f(40.f, 80.f)) // placeholder size - replace with sprite dims
        , m_textureCache(textureCache)
        , m_sprite(textureCache.get("assets/animations/characters/gideon/idle/Gideon_idle_01.png"))
    {
        // Visual
        m_shape.setSize(m_size);
        m_shape.setFillColor(sf::Color(180, 60, 60));   // placeholder red
        m_shape.setOutlineColor(sf::Color(220, 100, 100));
        m_shape.setOutlineThickness(1.f);

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

        m_wasGrounded = m_grounded;

        handleInput(input);
        applyGravity(dt);     
        applyMovement(dt);     
        resolveCollision(tileMap); 
        updateCoyote();
        updateAnimator(input);
        m_animator.update(dt);
    }

    //Input
    void Player::handleInput(Input& input)
    {
        m_jumpHeld = input.isHeld(Action::Jump);

        // Turn logic
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

        // Horizontal movement
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

        // Crouch
        m_crouching = input.isHeld(Action::Crouch) && m_grounded;

        // Jump
        if (canJump() && input.wasRecentlyPressed(Action::Jump))
        {
            jump();
            input.consumeBufferedAction(Action::Jump); // consume so it doesn't re-fire
        }
    }

    // Physics
    void Player::applyGravity(float dt)
    {
        if (!m_grounded)
        {
            // variable height only on first jump
            const bool canScale = m_jumpHeld
                && m_jumpState == JumpState::FirstJump
                && m_velocity.y < k_jumpHoldMaxVelocity;

            const float gravityScale = canScale ? k_jumpHoldGravityScale : 1.f;

            m_velocity.y += k_gravity * gravityScale * dt;
            m_velocity.y = std::min(m_velocity.y, k_maxFallSpeed);
        }
    }

    void Player::applyMovement(float dt)
    {
        m_position += m_velocity * dt;
        m_shape.setPosition(m_position);
    }

    //Collision
    void Player::resolveCollision(const TileMap& tileMap)
    {
        m_grounded = false;

        //X pass
        sf::FloatRect bounds = getBounds();
        auto solidTiles = tileMap.getSolidBounds(bounds);

        for (const sf::FloatRect& tile : solidTiles)
        {
            sf::FloatRect playerBounds = getBounds();
            auto intersection = playerBounds.findIntersection(tile);
            if (!intersection.has_value()) continue;

            const sf::FloatRect overlap = intersection.value();

            if (overlap.size.x <= overlap.size.y)
            {
                if (m_position.x < tile.position.x)
                    m_position.x -= overlap.size.x;
                else
                    m_position.x += overlap.size.x;

                m_velocity.x = 0.f;
            }
        }

        //Y pass
        bounds = getBounds();
        solidTiles = tileMap.getSolidBounds(bounds);

        for (const sf::FloatRect& tile : solidTiles)
        {
            sf::FloatRect playerBounds = getBounds();
            auto intersection = playerBounds.findIntersection(tile);
            if (!intersection.has_value()) continue;

            const sf::FloatRect overlap = intersection.value();

            if (overlap.size.y <= overlap.size.x)
            {
                if (m_position.y < tile.position.y)
                {
                    m_position.y = tile.position.y - m_size.y; // hard snap, no drift
                    m_velocity.y = 0.f;
                    m_grounded = true;
                    m_jumpState = JumpState::Grounded;
                    m_hasDoubleJump = true;  // restore double jump on land
                }
                else
                {
                    m_position.y += overlap.size.y;
                    m_velocity.y = 0.f;
                }
            }
        }

        // after both passes in resolveCollision, before m_shape.setPosition
        // probe downward by a small amount to confirm grounding
        if (!m_grounded)
        {
            sf::FloatRect probeRect(
                sf::Vector2f(m_position.x, m_position.y + 2.f),
                m_size
            );
            auto probeTiles = tileMap.getSolidBounds(probeRect);
            for (const sf::FloatRect& tile : probeTiles)
            {
                auto intersection = probeRect.findIntersection(tile);
                if (intersection.has_value())
                {
                    m_grounded = true;
                    break;
                }
            }
        }

        m_shape.setPosition(m_position);

        // Coyote time
        if (!m_grounded && m_jumpState == JumpState::Grounded && m_wasGrounded)
            m_coyoteFrames = k_coyoteFrames;
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
        if (m_grounded || m_coyoteAvailable)
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
        const bool isDoubleJump = !m_grounded && !m_coyoteAvailable;

        if (isDoubleJump)
        {
            m_jumpState = JumpState::SecondJump;
            m_hasDoubleJump = false;
            m_velocity.y = k_playerJumpForce;
            m_jumpHeld = false; // ignore hold on second jump
        }
        else
        {
            m_jumpState = JumpState::FirstJump;
            m_velocity.y = k_playerJumpForce;
            m_jumpHeld = false; // reset so hold scaling starts fresh
        }

        m_grounded = false;
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

        // Attack - check before airborne so air attack can trigger
        if (input.isJustPressed(Action::AttackPrimary))
        {
            m_hasHit = false;
            if (m_grounded)
                m_animator.setState(AnimationState::AttackPrimary);
            else
                m_animator.setState(AnimationState::AttackAir);
            return;
        }

        // if attack is playing - let it finish
        if ((current == AnimationState::AttackPrimary ||
            current == AnimationState::AttackAir) && !m_animator.isComplete())
            return;

        // Airborne
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
        if (m_grounded && !m_wasGrounded)
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
                m_position.x + m_size.x * 0.5f,
                m_position.y + m_size.y * 0.5f
            ));
            window.draw(m_sprite);

#ifdef _DEBUG
            // DEBUG - draw collision box outline - visualise trigger
            sf::RectangleShape debugBox;
            debugBox.setSize(m_size);
            debugBox.setPosition(m_position);
            debugBox.setFillColor(sf::Color::Transparent);
            debugBox.setOutlineColor(sf::Color::Green);
            debugBox.setOutlineThickness(1.f);
            window.draw(debugBox);
            
            // draw attack hitbox
            const auto hitbox = getAttackHitbox();
            if (hitbox.has_value())
            {
                sf::RectangleShape hitboxShape;
                hitboxShape.setSize(hitbox->size);
                hitboxShape.setPosition(hitbox->position);
                hitboxShape.setFillColor(sf::Color(255, 0, 0, 60));
                hitboxShape.setOutlineColor(sf::Color::Red);
                hitboxShape.setOutlineThickness(1.f);
                window.draw(hitboxShape);
            }
#endif
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
            ? m_position.x + (m_size.x * 0.5f) + k_attackHitboxWidth
            : m_position.x - (m_size.x * 0.5f) - k_attackHitboxWidth;

        return sf::FloatRect(
            sf::Vector2f(x, m_position.y),
            sf::Vector2f(k_attackHitboxWidth, k_attackHitboxHeight)
        );
    }

}