#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include "AnimationState.h"

namespace Metroidvania {

    struct AnimationClip {
        std::vector<std::string> frames;
        bool                     loop = true;
        float                    fps = 12.f;
        bool                     interruptible = true;   // can another state cut this one short
        int                      interruptFrame = 0;     // earliest frame this clip can be interrupted
    };

    class Animator {
    public:

        void addClip(AnimationState state, AnimationClip clip);

        // --- State control ---
        void setState(AnimationState state);  // respects interrupt rules
        void forceState(AnimationState state);  // bypasses interrupt rules - for Hurt, Die etc.
        void setReturnState(AnimationState state); // where non-looping clips return to when complete

        // --- Update ---
        void update(float dt);

        // --- Queries ---
        const std::string& getCurrentFrame() const;
        AnimationState     getState()        const { return m_currentState; }
        bool               isComplete()      const { return m_complete; }
        bool               isInterruptible() const;
        int                getCurrentFrameIndex() const { return m_frameIndex; }

        // --- Flip ---
        bool isFacingRight()  const { return m_facingRight; }
        void setFacingRight(bool facingRight) { m_facingRight = facingRight; }

    private:
        std::unordered_map<AnimationState, AnimationClip> m_clips;

        AnimationState m_currentState = AnimationState::IdlePassive;
        AnimationState m_returnState = AnimationState::IdlePassive;

        int   m_frameIndex = 0;
        float m_elapsed = 0.f;
        bool  m_complete = false;
        bool  m_facingRight = true;

        static const std::string k_emptyFrame;
    };

}