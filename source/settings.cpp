#include "settings/Settings.h"

namespace Settings {

    static mINI::INIMap<std::string> GetSection(const std::string& section, const mINI::INIStructure& ini)
    {
        if (ini.has(section)) {
            return ini.get(section);
        }
        return {};
    }

    static std::string Get(const std::string& section, const std::string& key, const mINI::INIStructure& ini)
    {
        auto sectionData = GetSection(section, ini);
        if (sectionData.has(key)) {
            return trim(sectionData.get(key));
        }
        return {};
    }

    template<typename T>
    static T GetValue(const std::string& section, const std::string& key, const mINI::INIStructure& ini, T defaultValue)
        requires std::is_arithmetic_v<T>
    {
        auto val = Get(section, key, ini);
        if (val.empty()) return defaultValue;
        if constexpr (std::is_same_v<T, bool>) {
            return val == "true" || val == "1" || val == "yes";
        } else if constexpr (std::is_floating_point_v<T>) {
            return static_cast<T>(std::stof(val));
        } else {
            return static_cast<T>(std::stoi(val));
        }
    }

    static void Set(mINI::INIStructure& ini, const std::string& section, const std::string& key, const std::string& value)
    {
        ini[section][key] = value;
    }

    template<typename T>
    static void SetValue(mINI::INIStructure& ini, const std::string& section, const std::string& key, T value)
    {
        if constexpr (std::is_same_v<T, bool>) {
            Set(ini, section, key, value ? "true" : "false");
        } else {
            Set(ini, section, key, std::to_string(value));
        }
    }

    // General
    bool General::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableMod = GetValue(section, "enableMod", ini, true);
        enableFirstPerson = GetValue(section, "enableFirstPerson", ini, true);
        enableThirdPerson = GetValue(section, "enableThirdPerson", ini, false);
        enableTransition = GetValue(section, "enableTransition", ini, true);
        enableHorse = GetValue(section, "enableHorse", ini, true);
        enableShip = GetValue(section, "enableShip", ini, true);
        enableFurniture = GetValue(section, "enableFurniture", ini, true);
        enableRagdoll = GetValue(section, "enableRagdoll", ini, false);
        enableDeathCinematic = GetValue(section, "enableDeathCinematic", ini, false);
        firstPersonWithBody = GetValue(section, "firstPersonWithBody", ini, false);
        auto profile = Get(section, "startingProfile", ini);
        startingProfile = profile.empty() ? "default" : profile;
        return true;
    }

    bool General::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableMod", enableMod);
        SetValue(ini, section, "enableFirstPerson", enableFirstPerson);
        SetValue(ini, section, "enableThirdPerson", enableThirdPerson);
        SetValue(ini, section, "enableTransition", enableTransition);
        SetValue(ini, section, "enableHorse", enableHorse);
        SetValue(ini, section, "enableShip", enableShip);
        SetValue(ini, section, "enableFurniture", enableFurniture);
        SetValue(ini, section, "enableRagdoll", enableRagdoll);
        SetValue(ini, section, "enableDeathCinematic", enableDeathCinematic);
        SetValue(ini, section, "firstPersonWithBody", firstPersonWithBody);
        Set(ini, section, "startingProfile", startingProfile);
        return true;
    }

    // Hide
    bool Hide::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        hideBodyFirstPerson = GetValue(section, "hideBodyFirstPerson", ini, false);
        hideHeadFirstPerson = GetValue(section, "hideHeadFirstPerson", ini, false);
        hideEquipmentFirstPerson = GetValue(section, "hideEquipmentFirstPerson", ini, false);
        showBodyInMenu = GetValue(section, "showBodyInMenu", ini, true);
        hideWeaponFirstPerson = GetValue(section, "hideWeaponFirstPerson", ini, false);
        hideWeaponThirdPerson = GetValue(section, "hideWeaponThirdPerson", ini, true);
        hideQuiverFirstPerson = GetValue(section, "hideQuiverFirstPerson", ini, false);
        hideMovementFirstPerson = GetValue(section, "hideMovementFirstPerson", ini, false);
        return true;
    }

    bool Hide::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "hideBodyFirstPerson", hideBodyFirstPerson);
        SetValue(ini, section, "hideHeadFirstPerson", hideHeadFirstPerson);
        SetValue(ini, section, "hideEquipmentFirstPerson", hideEquipmentFirstPerson);
        SetValue(ini, section, "showBodyInMenu", showBodyInMenu);
        SetValue(ini, section, "hideWeaponFirstPerson", hideWeaponFirstPerson);
        SetValue(ini, section, "hideWeaponThirdPerson", hideWeaponThirdPerson);
        SetValue(ini, section, "hideQuiverFirstPerson", hideQuiverFirstPerson);
        SetValue(ini, section, "hideMovementFirstPerson", hideMovementFirstPerson);
        return true;
    }

    // Fixes
    bool Fixes::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        fixShaderReferenceEffect = GetValue(section, "fixShaderReferenceEffect", ini, true);
        fixNearDistanceIndoors = GetValue(section, "fixNearDistanceIndoors", ini, true);
        fixHorseLookingDown = GetValue(section, "fixHorseLookingDown", ini, true);
        fixSmoothAnimationTransitions = GetValue(section, "fixSmoothAnimationTransitions", ini, true);
        fixNodeResetOnCellLoad = GetValue(section, "fixNodeResetOnCellLoad", ini, true);
        return true;
    }

    bool Fixes::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "fixShaderReferenceEffect", fixShaderReferenceEffect);
        SetValue(ini, section, "fixNearDistanceIndoors", fixNearDistanceIndoors);
        SetValue(ini, section, "fixHorseLookingDown", fixHorseLookingDown);
        SetValue(ini, section, "fixSmoothAnimationTransitions", fixSmoothAnimationTransitions);
        SetValue(ini, section, "fixNodeResetOnCellLoad", fixNodeResetOnCellLoad);
        return true;
    }

    // RestrictAngles
    bool RestrictAngles::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableRestrictAngles = GetValue(section, "enableRestrictAngles", ini, true);
        restrictSitting = GetValue(section, "restrictSitting", ini, true);
        restrictMounted = GetValue(section, "restrictMounted", ini, true);
        restrictFlying = GetValue(section, "restrictFlying", ini, true);
        restrictTransformed = GetValue(section, "restrictTransformed", ini, true);
        sittingMinPitch = GetValue(section, "sittingMinPitch", ini, -60.0f);
        sittingMaxPitch = GetValue(section, "sittingMaxPitch", ini, 60.0f);
        sittingMinYaw = GetValue(section, "sittingMinYaw", ini, -360.0f);
        sittingMaxYaw = GetValue(section, "sittingMaxYaw", ini, 360.0f);
        mountedMinPitch = GetValue(section, "mountedMinPitch", ini, -60.0f);
        mountedMaxPitch = GetValue(section, "mountedMaxPitch", ini, 60.0f);
        mountedMinYaw = GetValue(section, "mountedMinYaw", ini, -360.0f);
        mountedMaxYaw = GetValue(section, "mountedMaxYaw", ini, 360.0f);
        flyingMinPitch = GetValue(section, "flyingMinPitch", ini, -89.0f);
        flyingMaxPitch = GetValue(section, "flyingMaxPitch", ini, 89.0f);
        transformedMinPitch = GetValue(section, "transformedMinPitch", ini, -89.0f);
        transformedMaxPitch = GetValue(section, "transformedMaxPitch", ini, 89.0f);
        return true;
    }

    bool RestrictAngles::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableRestrictAngles", enableRestrictAngles);
        SetValue(ini, section, "restrictSitting", restrictSitting);
        SetValue(ini, section, "restrictMounted", restrictMounted);
        SetValue(ini, section, "restrictFlying", restrictFlying);
        SetValue(ini, section, "restrictTransformed", restrictTransformed);
        SetValue(ini, section, "sittingMinPitch", sittingMinPitch);
        SetValue(ini, section, "sittingMaxPitch", sittingMaxPitch);
        SetValue(ini, section, "sittingMinYaw", sittingMinYaw);
        SetValue(ini, section, "sittingMaxYaw", sittingMaxYaw);
        SetValue(ini, section, "mountedMinPitch", mountedMinPitch);
        SetValue(ini, section, "mountedMaxPitch", mountedMaxPitch);
        SetValue(ini, section, "mountedMinYaw", mountedMinYaw);
        SetValue(ini, section, "mountedMaxYaw", mountedMaxYaw);
        SetValue(ini, section, "flyingMinPitch", flyingMinPitch);
        SetValue(ini, section, "flyingMaxPitch", flyingMaxPitch);
        SetValue(ini, section, "transformedMinPitch", transformedMinPitch);
        SetValue(ini, section, "transformedMaxPitch", transformedMaxPitch);
        return true;
    }

    // Events
    Events::Events()
    {
        eventStates = {
            { "WeaponSwing", true },
            { "Footstep", true },
            { "JumpLand", true },
            { "Mounted", true },
            { "ShipTravel", true },
            { "Dialogue", true },
        };
    }

    bool Events::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableOverrideEvents = GetValue(section, "enableOverrideEvents", ini, true);
        for (auto& [eventName, state] : eventStates) {
            state = GetValue(section, eventName, ini, state);
        }
        return true;
    }

    bool Events::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableOverrideEvents", enableOverrideEvents);
        for (const auto& [eventName, state] : eventStates) {
            SetValue(ini, section, eventName, state);
        }
        return true;
    }

    // FOV
    FOV::FOV()
    {
        eventFOVs = {
            { "WeaponSwing", 80.0f },
            { "Footstep", 80.0f },
            { "JumpLand", 80.0f },
            { "Mounted", 90.0f },
            { "ShipTravel", 100.0f },
        };
    }

    bool FOV::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableOverrideFOV = GetValue(section, "enableOverrideFOV", ini, false);
        enableEventFOV = GetValue(section, "enableEventFOV", ini, false);
        firstPersonFOV = GetValue(section, "firstPersonFOV", ini, 80.0f);
        thirdPersonFOV = GetValue(section, "thirdPersonFOV", ini, 80.0f);
        horseFOV = GetValue(section, "horseFOV", ini, 80.0f);
        shipFOV = GetValue(section, "shipFOV", ini, 90.0f);
        furnitureFOV = GetValue(section, "furnitureFOV", ini, 80.0f);
        for (auto& [eventName, fov] : eventFOVs) {
            fov = GetValue(section, eventName, ini, fov);
        }
        return true;
    }

    bool FOV::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableOverrideFOV", enableOverrideFOV);
        SetValue(ini, section, "enableEventFOV", enableEventFOV);
        SetValue(ini, section, "firstPersonFOV", firstPersonFOV);
        SetValue(ini, section, "thirdPersonFOV", thirdPersonFOV);
        SetValue(ini, section, "horseFOV", horseFOV);
        SetValue(ini, section, "shipFOV", shipFOV);
        SetValue(ini, section, "furnitureFOV", furnitureFOV);
        for (const auto& [eventName, fov] : eventFOVs) {
            SetValue(ini, section, eventName, fov);
        }
        return true;
    }

    // NearDistance
    bool NearDistance::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableNearDistance = GetValue(section, "enableNearDistance", ini, true);
        nearDistance = GetValue(section, "nearDistance", ini, 15.0f);
        nearDistancePitchThreshold = GetValue(section, "nearDistancePitchThreshold", ini, 0.1f);
        enableNearDistanceEvent = GetValue(section, "enableNearDistanceEvent", ini, true);
        return true;
    }

    bool NearDistance::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableNearDistance", enableNearDistance);
        SetValue(ini, section, "nearDistance", nearDistance);
        SetValue(ini, section, "nearDistancePitchThreshold", nearDistancePitchThreshold);
        SetValue(ini, section, "enableNearDistanceEvent", enableNearDistanceEvent);
        return true;
    }

    // Headbob
    bool Headbob::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableHeadbob = GetValue(section, "enableHeadbob", ini, true);
        enableHeadbobTranslation = GetValue(section, "enableHeadbobTranslation", ini, true);
        enableHeadbobRotation = GetValue(section, "enableHeadbobRotation", ini, true);
        enableHeadbobSneaking = GetValue(section, "enableHeadbobSneaking", ini, true);
        enableHeadbobSprinting = GetValue(section, "enableHeadbobSprinting", ini, true);
        enableHeadbobSwimming = GetValue(section, "enableHeadbobSwimming", ini, true);
        enableHeadbobWalking = GetValue(section, "enableHeadbobWalking", ini, true);
        enableHeadbobRunning = GetValue(section, "enableHeadbobRunning", ini, true);
        enableHeadbobSiding = GetValue(section, "enableHeadbobSiding", ini, true);
        enableHeadbobSprintingSiding = GetValue(section, "enableHeadbobSprintingSiding", ini, true);
        headbobFrequency = GetValue(section, "headbobFrequency", ini, 1.0f);
        headbobIntensity = GetValue(section, "headbobIntensity", ini, 1.0f);
        headbobSneakMultiplier = GetValue(section, "headbobSneakMultiplier", ini, 0.5f);
        headbobSprintMultiplier = GetValue(section, "headbobSprintMultiplier", ini, 1.5f);
        headbobSwimMultiplier = GetValue(section, "headbobSwimMultiplier", ini, 1.2f);
        return true;
    }

    bool Headbob::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableHeadbob", enableHeadbob);
        SetValue(ini, section, "enableHeadbobTranslation", enableHeadbobTranslation);
        SetValue(ini, section, "enableHeadbobRotation", enableHeadbobRotation);
        SetValue(ini, section, "enableHeadbobSneaking", enableHeadbobSneaking);
        SetValue(ini, section, "enableHeadbobSprinting", enableHeadbobSprinting);
        SetValue(ini, section, "enableHeadbobSwimming", enableHeadbobSwimming);
        SetValue(ini, section, "enableHeadbobWalking", enableHeadbobWalking);
        SetValue(ini, section, "enableHeadbobRunning", enableHeadbobRunning);
        SetValue(ini, section, "enableHeadbobSiding", enableHeadbobSiding);
        SetValue(ini, section, "enableHeadbobSprintingSiding", enableHeadbobSprintingSiding);
        SetValue(ini, section, "headbobFrequency", headbobFrequency);
        SetValue(ini, section, "headbobIntensity", headbobIntensity);
        SetValue(ini, section, "headbobSneakMultiplier", headbobSneakMultiplier);
        SetValue(ini, section, "headbobSprintMultiplier", headbobSprintMultiplier);
        SetValue(ini, section, "headbobSwimMultiplier", headbobSwimMultiplier);
        return true;
    }

    // Camera
    bool Camera::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enableCameraOverrides = GetValue(section, "enableCameraOverrides", ini, true);
        translationSmoothing = GetValue(section, "translationSmoothing", ini, 0.1f);
        rotationSmoothing = GetValue(section, "rotationSmoothing", ini, 0.1f);
        hideWeaponInMenu = GetValue(section, "hideWeaponInMenu", ini, true);
        disableAutoVanity = GetValue(section, "disableAutoVanity", ini, false);
        return true;
    }

    bool Camera::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enableCameraOverrides", enableCameraOverrides);
        SetValue(ini, section, "translationSmoothing", translationSmoothing);
        SetValue(ini, section, "rotationSmoothing", rotationSmoothing);
        SetValue(ini, section, "hideWeaponInMenu", hideWeaponInMenu);
        SetValue(ini, section, "disableAutoVanity", disableAutoVanity);
        return true;
    }

    // PseudoFP
    bool PseudoFP::Load(const std::string& section, const mINI::INIStructure& ini)
    {
        enablePseudoFP = GetValue(section, "enablePseudoFP", ini, false);
        headOffsetX = GetValue(section, "headOffsetX", ini, 0.0f);
        headOffsetY = GetValue(section, "headOffsetY", ini, 0.0f);
        headOffsetZ = GetValue(section, "headOffsetZ", ini, 0.0f);
        cameraDistance = GetValue(section, "cameraDistance", ini, 0.0f);
        cameraHeight = GetValue(section, "cameraHeight", ini, 0.0f);
        return true;
    }

    bool PseudoFP::Save(const std::string& section, mINI::INIStructure& ini)
    {
        SetValue(ini, section, "enablePseudoFP", enablePseudoFP);
        SetValue(ini, section, "headOffsetX", headOffsetX);
        SetValue(ini, section, "headOffsetY", headOffsetY);
        SetValue(ini, section, "headOffsetZ", headOffsetZ);
        SetValue(ini, section, "cameraDistance", cameraDistance);
        SetValue(ini, section, "cameraHeight", cameraHeight);
        return true;
    }
}
