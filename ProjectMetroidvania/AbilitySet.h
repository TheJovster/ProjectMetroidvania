#pragma once
#include <array>
#include <memory>
#include <string>

namespace Metroidvania {

    // --- Ability base class ---
    class Ability {
    public:
        explicit Ability(std::string name)
            : m_name(std::move(name))
            , m_unlocked(false)
        {
        }

        virtual ~Ability() = default;

        virtual void activate() = 0;
        virtual void update(float dt) = 0;

        bool        isUnlocked() const { return m_unlocked; }
        bool        isOnCooldown() const { return m_cooldown > 0.f; }
        float       getCooldown()  const { return m_cooldown; }
        const std::string& getName() const { return m_name; }

        void unlock() { m_unlocked = true; }
        void lock() { m_unlocked = false; }

    protected:
        std::string m_name;
        bool        m_unlocked = false;
        float       m_cooldown = 0.f;      // counts down to zero
        float       m_maxCooldown = 0.f;
    };

    // --- Ability slots ---
    enum class AbilitySlot {
        DoubleJump,     // classic metroidvania unlock
        BackDash,       // SotN backdash with invincibility
        WallGrab,       // grab and climb walls
        Transform,      // bat / wolf / mist - SotN
        Primary,        // general purpose slot 1
        Secondary,      // general purpose slot 2

        COUNT
    };

    // --- AbilitySet - owned by Player ---
    class AbilitySet {
    public:
        AbilitySet() = default;

        void assign(AbilitySlot slot, std::shared_ptr<Ability> ability);
        void unassign(AbilitySlot slot);

        // Returns nullptr if slot is empty or ability is locked
        Ability* get(AbilitySlot slot) const;

        bool hasUnlocked(AbilitySlot slot) const;

        void update(float dt);

    private:
        static constexpr int k_abilitySlotCount = static_cast<int>(AbilitySlot::COUNT);
        std::array<std::shared_ptr<Ability>, k_abilitySlotCount> m_slots = {};
    };
}