#pragma once

namespace Metroidvania {

    enum class Action {
        // --- Movement ---
        MoveLeft,
        MoveRight,
        Crouch,
        Jump,

        // --- Combat ---
        AttackPrimary,    // right hand / main weapon
        AttackSecondary,  // left hand / shield or off-hand

        // --- Abilities ---
        AbilityPrimary,   // first special slot (sub-weapon, spell etc.)
        AbilitySecondary, // second special slot

        // --- Interaction ---
        Interact,

        // --- Meta ---
        Pause,

        COUNT
    };

}