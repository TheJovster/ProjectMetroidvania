#pragma once
#include "Animator.h"
#include <string>
#include <iomanip>
#include <sstream>

namespace Metroidvania {

    // builds an AnimationClip from a folder of sequentially named PNGs
    inline AnimationClip buildClip(
        const std::string& folder,
        const std::string& prefix,
        int                frameCount,
        bool               loop,
        float              fps,
        bool               interruptible = true,
        int                interruptFrame = 0)
    {
        AnimationClip clip;
        clip.loop = loop;
        clip.fps = fps;
        clip.interruptible = interruptible;
        clip.interruptFrame = interruptFrame;

        for (int i = 1; i <= frameCount; ++i)
        {
            // zero pad to two digits - 01, 02 ... 24
            std::ostringstream ss;
            ss << std::setw(2) << std::setfill('0') << i;
            clip.frames.push_back(folder + prefix + ss.str() + ".png");
        }

        return clip;
    }

}