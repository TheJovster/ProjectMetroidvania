#pragma once
#include "AbilitySet.h"

namespace Metroidvania {

    class DoubleJumpAbility : public Ability {
    public:
        DoubleJumpAbility()
            : Ability("Double Jump")
        {
        }

        void activate() override {}  // handled directly by Player
        void update(float dt) override {}  // no cooldown yet
    };

}