#pragma once

#include "RE/P/PlayerCamera.h"
#include "RE/M/Main.h"
#include "RE/N/NiCamera.h"
#include "RE/N/NiNode.h"
namespace Patch::Helper {

    inline RE::PlayerCamera* GetPlayerCamera()
    {
        return RE::PlayerCamera::GetSingleton();
    }

    inline RE::Main* GetMain()
    {
        return RE::Main::GetSingleton();
    }

    // NOTE: Do NOT add a GetNiCamera() wrapper around RE::Main::GetWorldRootCamera().
    // ID::Main::WorldRoot is unfilled (REL::ID{0}) in this CommonLibSF build and
    // resolving it crashes the game (hit this exact crash on save-load testing).
    // Find the active NiCamera by walking TESCamera::cameraRoot->children instead
    // (see EventsStarfield.cpp), re-resolving it every call rather than caching -
    // cameraRoot can be replaced by the engine (PlayerCamera::SetCameraRoot is
    // virtual) when the camera state changes.

    inline bool IsFirstPerson()
    {
        auto camera = GetPlayerCamera();
        return camera && camera->IsInFirstPerson();
    }

    inline bool IsThirdPerson()
    {
        auto camera = GetPlayerCamera();
        return camera && camera->IsInThirdPerson();
    }

    inline RE::CameraState GetCameraState()
    {
        auto camera = GetPlayerCamera();
        if (!camera || !camera->currentState)
            return RE::CameraState::kFirstPerson;
        return static_cast<RE::CameraState>(camera->QCameraEquals(RE::CameraState::kFirstPerson) ? 0 : 1);
    }

}