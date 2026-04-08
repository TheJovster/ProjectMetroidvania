#include "Animator.h"

namespace Metroidvania {

    const std::string Animator::k_emptyFrame = "";

    void Animator::addClip(AnimationState state, AnimationClip clip)
    {
        m_clips[state] = std::move(clip);
    }

    void Animator::setState(AnimationState state)
    {
        // Ignore states with no registered clip
        if (m_clips.find(state) == m_clips.end()) return;

        // Don't interrupt a non-interruptible clip before its interrupt frame
        if (!m_complete)
        {
            auto it = m_clips.find(m_currentState);
            if (it != m_clips.end())
            {
                const AnimationClip& current = it->second;

                // Non-looping and not interruptible at all - let it finish
                if (!current.loop && !current.interruptible)
                    return;

                // Interruptible but hasn't reached interrupt frame yet
                if (!current.loop && current.interruptible
                    && m_frameIndex < current.interruptFrame)
                    return;
            }
        }

        // Same state, not complete - don't restart
        if (state == m_currentState && !m_complete) return;

        m_currentState = state;
        m_frameIndex = 0;
        m_elapsed = 0.f;
        m_complete = false;
    }

    void Animator::forceState(AnimationState state)
    {
        // Bypasses all interrupt rules - use for Hurt, Die, forced transitions
        if (m_clips.find(state) == m_clips.end()) return;

        m_currentState = state;
        m_frameIndex = 0;
        m_elapsed = 0.f;
        m_complete = false;
    }

    void Animator::setReturnState(AnimationState state)
    {
        if (m_clips.find(state) == m_clips.end()) return;
        m_returnState = state;
    }

    void Animator::update(float dt)
    {
        m_justCompleted = false;  // clear each frame

        auto it = m_clips.find(m_currentState);
        if (it == m_clips.end()) return;

        const AnimationClip& clip = it->second;
        if (clip.frames.empty()) return;

        // Don't advance a completed non-looping clip
        if (m_complete) return;

        m_elapsed += dt;
        const float frameDuration = 1.f / clip.fps;

        while (m_elapsed >= frameDuration)
        {
            m_elapsed -= frameDuration;
            m_frameIndex++;

            if (m_frameIndex >= static_cast<int>(clip.frames.size()))
            {
                if (clip.loop)
                {
                    m_frameIndex = 0;
                }
                else
                {
                    m_frameIndex = static_cast<int>(clip.frames.size()) - 1;
                    m_complete = true;
                    m_lastCompletedState = m_currentState;  // save before auto-return
                    m_justCompleted = true;
                    m_currentState = m_returnState;
                    m_frameIndex = 0;
                    m_elapsed = 0.f;
                    return;
                }
            }
        }
    }

    const std::string& Animator::getCurrentFrame() const
    {
        auto it = m_clips.find(m_currentState);
        if (it == m_clips.end() || it->second.frames.empty())
            return k_emptyFrame;

        return it->second.frames[m_frameIndex];
    }

    bool Animator::isInterruptible() const
    {
        auto it = m_clips.find(m_currentState);
        if (it == m_clips.end()) return true;

        const AnimationClip& clip = it->second;

        // Looping clips are always interruptible
        if (clip.loop) return true;

        // Non-looping - check flag and frame window
        if (!clip.interruptible) return false;

        return m_frameIndex >= clip.interruptFrame;
    }

}