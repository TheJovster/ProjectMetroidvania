#include "AbilitySet.h"

namespace Metroidvania {

    void AbilitySet::assign(AbilitySlot slot, std::shared_ptr<Ability> ability)
    {
        m_slots[static_cast<int>(slot)] = std::move(ability);
    }

    void AbilitySet::unassign(AbilitySlot slot)
    {
        m_slots[static_cast<int>(slot)] = nullptr;
    }

    Ability* AbilitySet::get(AbilitySlot slot) const
    {
        const auto& ptr = m_slots[static_cast<int>(slot)];
        if (!ptr || !ptr->isUnlocked()) return nullptr;
        return ptr.get();
    }

    bool AbilitySet::hasUnlocked(AbilitySlot slot) const
    {
        const auto& ptr = m_slots[static_cast<int>(slot)];
        if (!ptr) return false;
        return ptr->isUnlocked();
    }

    void AbilitySet::update(float dt)
    {
        for (auto& slot : m_slots)
        {
            if (slot && slot->isUnlocked())
                slot->update(dt);
        }
    }

}