#pragma once

namespace Metroidvania {

    enum class AnimationState {

        // Idle
        IdlePassive,        // standing still, no threat
        IdleCombat,         // standing still, weapon drawn

        // Movement
        Walk,
        Run,
        BackDash,           // SotN backdash - brief invincibility window
        Turn,               // plays once, then flips and resumes movement

        // Airborne
        JumpAscent,         // rising
        JumpApex,           // brief float at top - optional, adds game feel
        Fall,               // descending
        Land,               // one-shot, returns to idle or walk

        // Crouch
        Crouch,             // transitioning down
        CrouchIdle,         // held crouch
        CrouchRise,         // transitioning back up

        // Combat
        AttackPrimary,      // right hand - weapon dependent later
        AttackSecondary,    // left hand - shield / off-hand
        AttackAir,          // attacking while airborne

        // Abilities
        Cast,               // spell / ability activation
        Transform,          // bat / wolf / mist - SotN transformations

        // Hurt / Death
        Hurt,               // hit reaction - brief, interruptible by nothing
        Die,                // one-shot, leads to Dead
        Dead,               // hold last frame

        // Interaction
        Interact,           // chest / door / NPC

        COUNT
    };

}