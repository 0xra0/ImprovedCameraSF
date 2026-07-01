#pragma once

namespace RE
{
    class Actor;
}

namespace SAFIntegration
{
    /// Checks if the SAF (Starfield Animation Framework) DLL is loaded and
    /// currently playing an animation on the given actor.
    /// Returns false if SAF is not present or not animating the actor.
    bool IsSAFAnimationPlaying(RE::Actor* a_actor);
}
