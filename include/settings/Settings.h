#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>

#include <mini/ini.h>

namespace Settings {

    static inline std::string trim(const std::string& str)
    {
        auto start = str.begin();
        while (start != str.end() && std::isspace(*start)) {
            start++;
        }
        auto end = str.end();
        do {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));
        return std::string(start, end + 1);
    }

    class ISetting {
    public:
        virtual ~ISetting() = default;
        virtual bool Load(const std::string& section, const mINI::INIStructure& ini) = 0;
        virtual bool Save(const std::string& section, mINI::INIStructure& ini) = 0;
    };

    struct General : ISetting {
        bool enableMod = true;
        bool enableFirstPerson = true;
        bool enableThirdPerson = false;
        bool enableTransition = true;
        bool enableHorse = true;
        bool enableShip = true;
        bool enableFurniture = true;
        bool enableRagdoll = false;
        bool enableDeathCinematic = false;
        bool firstPersonWithBody = false;
        std::string startingProfile = "default";

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct Hide : ISetting {
        bool hideBodyFirstPerson = false;
        bool hideHeadFirstPerson = false;
        bool hideEquipmentFirstPerson = false;
        bool showBodyInMenu = true;
        bool hideWeaponFirstPerson = false;
        bool hideWeaponThirdPerson = true;
        bool hideQuiverFirstPerson = false;
        bool hideMovementFirstPerson = false;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct Fixes : ISetting {
        bool fixShaderReferenceEffect = true;
        bool fixNearDistanceIndoors = true;
        bool fixHorseLookingDown = true;
        bool fixSmoothAnimationTransitions = true;
        bool fixNodeResetOnCellLoad = true;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct RestrictAngles : ISetting {
        bool enableRestrictAngles = true;
        bool restrictSitting = true;
        bool restrictMounted = true;
        bool restrictFlying = true;
        bool restrictTransformed = true;
        float sittingMinPitch = -60.0f;
        float sittingMaxPitch = 60.0f;
        float sittingMinYaw = -360.0f;
        float sittingMaxYaw = 360.0f;
        float mountedMinPitch = -60.0f;
        float mountedMaxPitch = 60.0f;
        float mountedMinYaw = -360.0f;
        float mountedMaxYaw = 360.0f;
        float flyingMinPitch = -89.0f;
        float flyingMaxPitch = 89.0f;
        float transformedMinPitch = -89.0f;
        float transformedMaxPitch = 89.0f;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct Events : ISetting {
        bool enableOverrideEvents = true;
        std::unordered_map<std::string, bool> eventStates;

        Events();
        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct FOV : ISetting {
        bool enableOverrideFOV = false;
        bool enableEventFOV = false;
        float firstPersonFOV = 80.0f;
        float thirdPersonFOV = 80.0f;
        float horseFOV = 80.0f;
        float shipFOV = 90.0f;
        float furnitureFOV = 80.0f;
        std::unordered_map<std::string, float> eventFOVs;

        FOV();
        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct NearDistance : ISetting {
        bool enableNearDistance = true;
        float nearDistance = 15.0f;
        float nearDistancePitchThreshold = 0.1f;
        bool enableNearDistanceEvent = true;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct Headbob : ISetting {
        bool enableHeadbob = true;
        bool enableHeadbobTranslation = true;
        bool enableHeadbobRotation = true;
        bool enableHeadbobSneaking = true;
        bool enableHeadbobSprinting = true;
        bool enableHeadbobSwimming = true;
        bool enableHeadbobWalking = true;
        bool enableHeadbobRunning = true;
        bool enableHeadbobSiding = true;
        bool enableHeadbobSprintingSiding = true;
        float headbobFrequency = 1.0f;
        float headbobIntensity = 1.0f;
        float headbobSneakMultiplier = 0.5f;
        float headbobSprintMultiplier = 1.5f;
        float headbobSwimMultiplier = 1.2f;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct Camera : ISetting {
        bool enableCameraOverrides = true;
        float translationSmoothing = 0.1f;
        float rotationSmoothing = 0.1f;
        bool hideWeaponInMenu = true;
        bool disableAutoVanity = false;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };

    struct PseudoFP : ISetting {
        bool enablePseudoFP = false;
        float headOffsetX = 0.0f;
        float headOffsetY = 0.0f;
        float headOffsetZ = 0.0f;
        float cameraDistance = 0.0f;
        float cameraHeight = 0.0f;

        bool Load(const std::string& section, const mINI::INIStructure& ini) override;
        bool Save(const std::string& section, mINI::INIStructure& ini) override;
    };
}
