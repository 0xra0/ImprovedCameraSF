#pragma once

#include "RE/N/NiAVObject.h"
#include "RE/N/NiPoint.h"
#include "RE/T/TESCamera.h"
#include "RE/N/NiCamera.h"

namespace Patch {
    extern bool g_PseudoFPPActive;
    extern bool g_SAFAnimationPlaying;
    RE::NiCamera* FindNiCamera(RE::TESCamera* tesCam);
    RE::NiAVObject* GetCachedHeadBone();
    bool ComputePseudoFPPWorldPosition(RE::TESCamera* tesCam, RE::NiPoint3& outWorldPos, RE::NiPoint3* outHeadAnchor = nullptr, bool* outUsingFallback = nullptr);
    bool ApplyPseudoFPPRig(RE::TESCamera* tesCam, void* tpsThis = nullptr);
    void RestoreCameraOrbit();
    void ResetPseudoFPPState(bool a_restoreOrbit = true);
    void ResetCameraNodesToPlayer();
    void ClearPseudoCameraPointers();

    class Hooks {
    public:
        Hooks();
        ~Hooks() = default;

        Hooks(const Hooks&) = delete;
        Hooks& operator=(const Hooks&) = delete;
        Hooks(Hooks&&) = delete;
        Hooks& operator=(Hooks&&) = delete;

        bool Setup();
        bool Remove();

    private:
        bool m_Setup = false;
    };
}
