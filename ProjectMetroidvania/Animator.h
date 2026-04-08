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

        //state control
        void setState(AnimationState state);  // respects interrupt rules
        void forceState(AnimationState state);  // bypasses interrupt rules - for Hurt, Die etc.
        void setReturnState(AnimationState state); // where non-looping clips return to when complete

        //update
        void update(float dt);

        //queries
        const std::string& getCurrentFrame() const;
        AnimationState     getState()        const { return m_currentState; }
        bool               isComplete()      const { return m_complete; }
        bool               isInterruptible() const;
        int                getCurrentFrameIndex() const { return m_frameIndex; }

        //flip
        bool isFacingRight()  const { return m_facingRight; }
        void setFacingRight(bool facingRight) { m_facingRight = facingRight; }

        bool           justCompleted()         const { return m_justCompleted; }
        AnimationState getLastCompletedState() const { return m_lastCompletedState; }

    private:
        std::unordered_map<AnimationState, AnimationClip> m_clips;

        AnimationState m_currentState = AnimationState::IdlePassive;
        AnimationState m_returnState = AnimationState::IdlePassive;

        bool           m_justCompleted = false;
        AnimationState m_lastCompletedState = AnimationState::IdlePassive;

        int   m_frameIndex = 0;
        float m_elapsed = 0.f;
        bool  m_complete = false;
        bool  m_facingRight = true;

        static const std::string k_emptyFrame;
    };

}