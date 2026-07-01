#include "starfield/SAFIntegration.h"
#include <Windows.h>

namespace SAFIntegration
{
    // Import function pointer type for SAF's IsPlayingAnimation
    using IsPlayingAnimationFunc = bool (*)(RE::Actor*);

    static IsPlayingAnimationFunc s_IsPlayingAnimation = nullptr;

    /// Attempts to resolve SAF API function pointers.
    /// Returns true if all required functions are available.
    static bool ResolveSAFApi()
    {
        if (s_IsPlayingAnimation)
            return true;  // already resolved

        HMODULE safModule = GetModuleHandleA("StarfieldAnimationFramework.dll");
        if (!safModule)
            return false;  // SAF not loaded

        s_IsPlayingAnimation = reinterpret_cast<IsPlayingAnimationFunc>(
            GetProcAddress(safModule, "SAFAPI_IsPlayingAnimation"));

        return s_IsPlayingAnimation != nullptr;
    }

    bool IsSAFAnimationPlaying(RE::Actor* a_actor)
    {
        if (!a_actor)
            return false;

        if (!ResolveSAFApi())
            return false;

        return s_IsPlayingAnimation(a_actor);
    }
}
