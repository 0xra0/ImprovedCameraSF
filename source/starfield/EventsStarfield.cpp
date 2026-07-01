#define _CRT_SECURE_NO_WARNINGS
#include "starfield/EventsStarfield.h"
#include "starfield/Hooks.h"
#include "starfield/Helper.h"
#include "starfield/SAFIntegration.h"
#include "SFSE/API.h"
#include "RE/P/PlayerCamera.h"
#include "RE/T/TESCamera.h"
#include "RE/N/NiCamera.h"
#include "RE/N/NiNode.h"
#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <Windows.h>

#pragma push_macro("near")
#pragma push_macro("far")
#undef near
#undef far

namespace Patch {

    static void VLog(const char* fmt, va_list args)
    {
        char* userProfile = nullptr;
        size_t len = 0;
        _dupenv_s(&userProfile, &len, "USERPROFILE");
        std::string path = std::string(userProfile ? userProfile : "") + "\\Documents\\My Games\\Starfield\\SFSE\\ImprovedCameraSF_debug.log";
        free(userProfile);
        std::ofstream log(path, std::ios::app);
        if (log) {
            time_t t = time(nullptr);
            struct tm tm;
            localtime_s(&tm, &t);
            char buf[64];
            strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
            log << "[" << buf << "] ";
            char msg[512];
            vsprintf_s(msg, fmt, args);
            log << msg << std::endl;
        }
    }

    static void LogFormatted(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        VLog(fmt, args);
        va_end(args);
    }

    static void Log(const char* msg) { LogFormatted("%s", msg); }

    static std::string GetPluginDir()
    {
        char path[MAX_PATH];
        GetModuleFileNameA(GetModuleHandleA("ImprovedCameraSF.dll"), path, MAX_PATH);
        std::string full(path);
        size_t pos = full.find_last_of("\\/");
        return (pos != std::string::npos) ? full.substr(0, pos + 1) : "";
    }

    static float GetINIFloat(const char* section, const char* key, float defaultValue)
    {
        std::string iniPath = GetPluginDir() + "ImprovedCameraSF.ini";
        char buf[32];
        GetPrivateProfileStringA(section, key, "", buf, sizeof(buf), iniPath.c_str());
        if (strlen(buf) == 0)
            return defaultValue;
        return static_cast<float>(atof(buf));
    }

    static void EnsureINI()
    {
        std::string iniPath = GetPluginDir() + "ImprovedCameraSF.ini";
        if (GetFileAttributesA(iniPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            WritePrivateProfileStringA("Settings", "fTPWorldFOV", "80.0", iniPath.c_str());
            WritePrivateProfileStringA("Settings", "fFPWorldFOV", "85.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fOffsetX", "0.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fOffsetY", "0.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fOffsetZ", "0.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fFOV", "85.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "bUltraRigidHeadAttach", "false", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fHeadAttachGraceFrames", "45", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fUltraRigidMaxPlanarOffset", "0.60", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fUltraRigidHeightTolerance", "0.50", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fMaxYawDegrees", "90", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fNoseForward", "0.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fForwardOffset", "0.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fSideOffset", "0.0", iniPath.c_str());
            WritePrivateProfileStringA("PseudoFP", "fUpOffset", "0.0", iniPath.c_str());
            Log("Created default ImprovedCameraSF.ini");
        }
    }

    EventsStarfield::EventsStarfield() {}

    bool EventsStarfield::Setup()
    {
        if (m_Setup)
            return true;

        Log("Start");
        EnsureINI();

        float tppFOV = GetINIFloat("Settings", "fTPWorldFOV", 84.0f);
        float fpFOV = GetINIFloat("Settings", "fFPWorldFOV", 85.0f);
        float pseudoFOV = GetINIFloat("PseudoFP", "fFOV", 85.0f);

        auto task = SFSE::GetTaskInterface();
        if (task) {
            task->AddPermanentTask([tppFOV, fpFOV, pseudoFOV]() {
                auto camera = RE::PlayerCamera::GetSingleton();
                if (!camera) return;

                // === SAF integration ===
                // If SAF is playing an animation on the player while in a
                // non-TPP state (kFurniture), we keep pseudo active so the
                // camera stays at the player's head. This requires SAF to
                // export SAFAPI_IsPlayingAnimation.
                auto* player = RE::PlayerCharacter::GetSingleton();
                bool safPlaying = false;
                static int safLogThrottle = 0;
                if (player) {
                    safPlaying = SAFIntegration::IsSAFAnimationPlaying(player);
                }
                Patch::g_SAFAnimationPlaying = safPlaying;
                safLogThrottle++;
                if (safPlaying && (safLogThrottle % 60) == 0) {
                    LogFormatted("PseudoFP: SAF animation detected on player");
                }

                // g_PseudoUserEnabled: user wants pseudo (set by F4)
                // g_PseudoFPPActive:  pseudo is actually running this frame
                static bool g_PseudoUserEnabled = false;

                // --- F4 toggle ---
                static bool wasF4Down = false;
                bool isF4Down = (GetAsyncKeyState(VK_F4) & 0x8000) != 0;
                if (isF4Down && !wasF4Down) {
                    g_PseudoUserEnabled = !g_PseudoUserEnabled;
                    LogFormatted("PseudoFP: user %s by F4", g_PseudoUserEnabled ? "ENABLED" : "DISABLED");
                    if (g_PseudoUserEnabled) {
                        camera->ForceThirdPerson();
                    } else {
                        if (g_PseudoFPPActive) {
                            g_PseudoFPPActive = false;
                            ResetPseudoFPPState();
                        }
                    }
                }
                wasF4Down = isF4Down;

                // --- Determine state ---
                const bool isTPP = camera->IsInThirdPerson();
                const bool isFPP = camera->IsInFirstPerson();
                const bool isFurniture = camera->QCameraEquals(RE::CameraState::kFurniture);
                const bool isBodyState = isTPP || isFPP || isFurniture;

                // --- Decide if pseudo should run this frame ---
                // Pseudo runs when:
                //   a) User enabled + TPP (normal pseudo)
                //   b) User enabled + SAF animation playing (even in kFurniture)
                // Otherwise pseudo is disabled to let the engine control the camera.
                bool shouldPseudo = false;
                if (g_PseudoUserEnabled) {
                    if (isTPP) {
                        shouldPseudo = true;
                    } else if (isFurniture) {
                        shouldPseudo = true;
                    } else if (!isFPP && safPlaying) {
                        // SAF animation keeps pseudo active in non-TPP states
                        shouldPseudo = true;
                    }
                    // Force FPP back to TPP
                    if (isFPP) {
                        camera->ForceThirdPerson();
                    }
                }

                // --- Apply or disable pseudo ---
                if (shouldPseudo) {
                    if (!g_PseudoFPPActive) {
                        g_PseudoFPPActive = true;
                        LogFormatted("PseudoFP: activated (TPP or SAF)");
                    }
                    // kFurniture is handled by DetourUpdate after origUpdate
                    // has processed mouse input into the orbit position,
                    // so we skip it here to avoid stale cr->world.translate.
                    if (!isFurniture) {
                        auto* tesCam = static_cast<RE::TESCamera*>(camera);
                        if (!ApplyPseudoFPPRig(tesCam, nullptr)) {
                            return;
                        }
                    }
                } else {
                    if (g_PseudoFPPActive) {
                        g_PseudoFPPActive = false;
                        ClearPseudoCameraPointers();
                        LogFormatted("PseudoFP: disabled (non-TPP without SAF)");
                    }
                    if (!isBodyState) {
                        // Skip TPP-specific logic below
                        return;
                    }
                }

                // --- Log camera state ---
                static int stateLogCounter = 0;
                stateLogCounter++;
                if ((stateLogCounter % 60) == 0) {
                    auto* tesCam = static_cast<RE::TESCamera*>(camera);
                    uint32_t stateIdx = 0xFF;
                    for (uint32_t i = 0; i < RE::CameraState::kTotal; i++) {
                        if (tesCam->currentState == camera->cameraStates[i]) {
                            stateIdx = i;
                            break;
                        }
                    }
                    LogFormatted("CAM_STATE: idx=%u TPP=%d FP=%d pseudo=%d user=%d saf=%d",
                        stateIdx, isTPP ? 1 : 0, isFPP ? 1 : 0,
                        g_PseudoFPPActive ? 1 : 0, g_PseudoUserEnabled ? 1 : 0,
                        safPlaying ? 1 : 0);
                }

                // --- FOV switching ---
                static int lastState = -1;
                int state = isTPP ? 1 : 0;
                if (state != lastState && !g_PseudoFPPActive) {
                    lastState = state;
                    auto prefs = RE::INIPrefSettingCollection::GetSingleton();
                    if (!prefs) return;
                    if (isTPP) {
                        prefs->SetSetting("fTPWorldFOV:Camera", tppFOV);
                        LogFormatted("TPP FOV set to %.1f", tppFOV);
                    } else {
                        prefs->SetSetting("fFPWorldFOV:Camera", fpFOV);
                        LogFormatted("FP FOV set to %.1f", fpFOV);
                    }
                }

                // Pseudo-FPP FOV
                static float lastPseudoFOV = -1.0f;
                if (g_PseudoFPPActive && std::fabs(pseudoFOV - lastPseudoFOV) > 0.01f) {
                    lastPseudoFOV = pseudoFOV;
                    auto prefs = RE::INIPrefSettingCollection::GetSingleton();
                    if (prefs) {
                        prefs->SetSetting("fTPWorldFOV:Camera", pseudoFOV);
                        LogFormatted("PseudoFP FOV set to %.1f", pseudoFOV);
                    }
                } else if (!g_PseudoFPPActive) {
                    lastPseudoFOV = -1.0f;
                }

            });
            Log("Task registered");
        }

        m_Setup = true;
        Log("Setup done");
        return true;
    }

    bool EventsStarfield::Remove()
    {
        m_Setup = false;
        return true;
    }

}

#pragma pop_macro("near")
#pragma pop_macro("far")
