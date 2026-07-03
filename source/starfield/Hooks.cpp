#include "starfield/Hooks.h"
#include "starfield/SAFIntegration.h"
#include "plugin.h"
#include "REL/Relocation.h"
#include "RE/P/PlayerCamera.h"
#include "RE/P/PlayerCharacter.h"
#include "RE/T/TESCamera.h"
#include "RE/T/TESObjectREFR.h"
#include "RE/N/NiCamera.h"
#include "RE/N/NiNode.h"
#include "RE/B/BSFixedString.h"
#include "RE/RTTI.h"
#include "RE/IDs_VTABLE.h"
#include <MinHook.h>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <algorithm>
#include <Windows.h>


namespace Patch {

    bool g_PseudoFPPActive = false;
    bool g_SAFAnimationPlaying = false;

    static void LogFormatted(const char* fmt, ...);
    static void HideHead(bool a_hide);

    static RE::NiAVObject* g_HeadBone = nullptr;
    static RE::NiAVObject* g_HeadAnchorNode = nullptr;
    static float g_EyeHeight = 1.4f;
    static bool g_HasEyeHeight = false;
    static bool g_WasHeadAppCulled = false;
    static bool g_HasSavedHeadCull = false;
    static RE::NiAVObject* g_HeadMesh = nullptr;
    static RE::NiPoint3 g_HeadAnchorLocalOffset = {};
    static RE::NiPoint3 g_LastValidHeadAnchorWorld = {};
    static bool g_HasLastValidHeadAnchorWorld = false;
    static uint32_t g_HeadAnchorMissCount = 0;
    static RE::NiAVObject* g_CameraRoot = nullptr;
    static RE::NiPoint3 g_PrevRootLocal = {};
    static RE::NiPoint3 g_PrevRootWorld = {};
    static RE::NiMatrix3 g_PrevRootLocalRot = {};
    static RE::NiMatrix3 g_PrevRootWorldRot = {};
    static RE::NiMatrix3 g_PrevRootPrevWorldRot = {};
    static bool g_RestoreRootRotation = false;
    static bool g_FurnitureYawLocked = false;
    static float g_FurnitureBaseYaw = 0.0f;
    static float g_FurnitureOrbitYaw = 0.0f;
    static RE::NiPoint3 g_PrevSetLocal = {};
    static RE::NiPoint3 g_PrevSetWorld = {};
    static RE::NiAVObject* g_PrevSkeletonRoot = nullptr;
    static RE::NiCamera* g_NiCamera = nullptr;
    float g_AngleBlendZ = 0.0f;
    static void* g_origUpdateWorldData = nullptr;
    static void* g_origUpdateTransformAndBounds = nullptr;
    static void* g_origUpdateTransforms = nullptr;


    namespace
    {
        static float GetPseudoFPBoneForwardOffset();
        static float GetPseudoFPBoneSideOffset();
        static float GetPseudoFPBoneUpOffset();
        static RE::NiPoint3 GetCurrentHeadAnchorWorldPosition();
        static RE::NiPoint3 GetHeadMeshCenter(RE::NiAVObject* headMesh);

        // Default eye offset (meters) from the pseudo-FP head anchor to the eye
        // point. The anchor resolves to the head bone / head-geometry CENTER:
        // Starfield exposes no "Eye_Target"/"faceBone_C_EyesFat" node, so
        // FindPreferredEyeNode falls back to the head-mesh center. With a zero
        // offset the camera sits inside the skull ("camera in head"); push it
        // forward toward the face and up toward the brow to reach the eyes.
        // Both stay overridable in [PseudoFP]: forward via fNoseForward /
        // fForwardOffset, up via fOffsetZ / fUpOffset.
        constexpr float kDefaultEyeForwardOffset = 0.12f;
        constexpr float kDefaultEyeUpOffset = 0.20f;  // ~eye level; 0.06 sat at jaw (saw own face)

        static const std::string& GetPluginDirIniPath()
        {
            static std::string iniPath;
            static bool initialized = false;
            static bool logged = false;

            if (!initialized) {
                char path[MAX_PATH];
                GetModuleFileNameA(GetModuleHandleA("ImprovedCameraSF.dll"), path, MAX_PATH);
                std::string full(path);
                size_t pos = full.find_last_of("\\/");
                std::string dir = (pos != std::string::npos) ? full.substr(0, pos + 1) : "";
                iniPath = dir + "ImprovedCameraSF.ini";
                initialized = true;
            }

            if (initialized && !logged) {
                // Log once so users can confirm they're editing the correct INI file.
                LogFormatted("INI_PATH: %s", iniPath.c_str());
                logged = true;
            }

            return iniPath;
        }

        static RE::NiPoint3 TransformLocalToWorld(const RE::NiMatrix3& rotation, const RE::NiPoint3& localOffset)
        {
            return {
                rotation.entry[0][0] * localOffset.x + rotation.entry[0][1] * localOffset.y + rotation.entry[0][2] * localOffset.z,
                rotation.entry[1][0] * localOffset.x + rotation.entry[1][1] * localOffset.y + rotation.entry[1][2] * localOffset.z,
                rotation.entry[2][0] * localOffset.x + rotation.entry[2][1] * localOffset.y + rotation.entry[2][2] * localOffset.z
            };
        }

        static RE::NiPoint3 TransformWorldToLocal(const RE::NiMatrix3& rotation, const RE::NiPoint3& worldOffset)
        {
            RE::NiMatrix3 invRot = rotation.Transpose();
            return {
                invRot.entry[0][0] * worldOffset.x + invRot.entry[0][1] * worldOffset.y + invRot.entry[0][2] * worldOffset.z,
                invRot.entry[1][0] * worldOffset.x + invRot.entry[1][1] * worldOffset.y + invRot.entry[1][2] * worldOffset.z,
                invRot.entry[2][0] * worldOffset.x + invRot.entry[2][1] * worldOffset.y + invRot.entry[2][2] * worldOffset.z
            };
        }

        static RE::NiPoint3 ComputeNodeLocalFromWorld(RE::NiAVObject* node, const RE::NiPoint3& desiredWorldPos)
        {
            if (!node || !node->parent) {
                return desiredWorldPos;
            }

            RE::NiPoint3 diff = desiredWorldPos - node->parent->world.translate;
            return TransformWorldToLocal(node->parent->world.rotate, diff);
        }

        static float ClampFloat(float value, float minValue, float maxValue)
        {
            return (std::max)(minValue, (std::min)(value, maxValue));
        }

        static float WrapAnglePi(float angle)
        {
            while (angle > 3.14159265f) angle -= 6.28318531f;
            while (angle < -3.14159265f) angle += 6.28318531f;
            return angle;
        }

        static RE::NiMatrix3 MakeYawMatrix(float yaw)
        {
            const float c = std::cos(yaw);
            const float s = std::sin(yaw);
            return RE::NiMatrix3(
                c, -s, 0.0f, 0.0f,
                s,  c, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f);
        }

        static float DistanceSquared(const RE::NiPoint3& a, const RE::NiPoint3& b)
        {
            const float dx = a.x - b.x;
            const float dy = a.y - b.y;
            const float dz = a.z - b.z;
            return dx * dx + dy * dy + dz * dz;
        }

        static bool IsFinitePoint(const RE::NiPoint3& point)
        {
            return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
        }

        template <typename Fn>
        static void VisitNodesByName(RE::NiAVObject* root, const char* nameSubstr, Fn&& callback)
        {
            if (!root) return;
            const char* nameStr = root->name.c_str();
            if (nameStr && std::strstr(nameStr, nameSubstr)) {
                callback(root);
            }
            auto* asNode = root->GetAsNiNode();
            if (asNode) {
                for (auto& child : asNode->children) {
                    if (child) {
                        VisitNodesByName(child.get(), nameSubstr, callback);
                    }
                }
            }
        }

        static bool NormalizePlanar(RE::NiPoint3& vector)
        {
            vector.z = 0.0f;
            const float len = std::sqrt(vector.x * vector.x + vector.y * vector.y);
            if (len <= 0.0001f) {
                return false;
            }
            vector.x /= len;
            vector.y /= len;
            return true;
        }

        static bool Normalize3D(RE::NiPoint3& vector)
        {
            const float len = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
            if (len <= 0.0001f) {
                return false;
            }
            vector.x /= len;
            vector.y /= len;
            vector.z /= len;
            return true;
        }

        static void GetYawAxes(float yaw, RE::NiPoint3& outForward, RE::NiPoint3& outRight)
        {
            const float c = std::cos(yaw);
            const float s = std::sin(yaw);
            // Standard CE convention: yaw=0 = face North (+Y),
            // yaw increases CCW (π/2 = face East +X).
            outForward = { s, c, 0.0f };
            outRight = { c, -s, 0.0f };
        }

        static void GetPlayerWorldAxes(RE::PlayerCharacter* player, RE::NiPoint3& outForward, RE::NiPoint3& outRight)
        {
            RE::NiAVObject* worldOrientationNode = g_PrevSkeletonRoot ? g_PrevSkeletonRoot : g_HeadBone;
            if (worldOrientationNode) {
                outForward = {
                    worldOrientationNode->world.rotate.entry[0][1],
                    worldOrientationNode->world.rotate.entry[1][1],
                    worldOrientationNode->world.rotate.entry[2][1]
                };
                outRight = {
                    worldOrientationNode->world.rotate.entry[0][0],
                    worldOrientationNode->world.rotate.entry[1][0],
                    worldOrientationNode->world.rotate.entry[2][0]
                };
                if (NormalizePlanar(outForward) && NormalizePlanar(outRight)) {
                    return;
                }
            }

            const float yaw = player ? player->GetAngleZ() : 0.0f;
            GetYawAxes(yaw, outForward, outRight);
        }

        static void GetPseudoFPAxes(RE::PlayerCharacter* player, RE::NiPoint3& outForward, RE::NiPoint3& outRight)
        {
            GetPlayerWorldAxes(player, outForward, outRight);
        }

        static void GetNodeAxes(RE::NiAVObject* node, RE::PlayerCharacter* player, RE::NiPoint3& outForward, RE::NiPoint3& outRight, RE::NiPoint3& outUp)
        {
            if (node) {
                outForward = {
                    node->world.rotate.entry[0][1],
                    node->world.rotate.entry[1][1],
                    node->world.rotate.entry[2][1]
                };
                outRight = {
                    node->world.rotate.entry[0][0],
                    node->world.rotate.entry[1][0],
                    node->world.rotate.entry[2][0]
                };
                outUp = {
                    node->world.rotate.entry[0][2],
                    node->world.rotate.entry[1][2],
                    node->world.rotate.entry[2][2]
                };
            } else {
                const float yaw = player ? player->GetAngleZ() : 0.0f;
                GetYawAxes(yaw, outForward, outRight);
                outUp = { 0.0f, 0.0f, 1.0f };
            }

            if (!Normalize3D(outForward)) {
                const float yaw = player ? player->GetAngleZ() : 0.0f;
                GetYawAxes(yaw, outForward, outRight);
            }
            if (!Normalize3D(outRight)) {
                outRight = { outForward.y, -outForward.x, 0.0f };
                Normalize3D(outRight);
            }
            if (!Normalize3D(outUp)) {
                outUp = { 0.0f, 0.0f, 1.0f };
            }
        }

        static RE::NiPoint3 ComputeUltraRigidHeadAnchorWorldPosition(RE::PlayerCharacter* player)
        {
            (void)player;
            if (g_HeadMesh) {
                RE::NiPoint3 meshCenter = GetHeadMeshCenter(g_HeadMesh);
                if (IsFinitePoint(meshCenter)) {
                    return meshCenter;
                }
            }

            if (!g_HeadBone) {
                return {};
            }
            return g_HeadBone->world.translate;
        }

        static RE::NiPoint3 ComputeUltraRigidCameraWorldPosition(RE::PlayerCharacter* player, const RE::NiPoint3& headAnchor, RE::NiAVObject* cr = nullptr)
        {
            RE::NiPoint3 worldPos = headAnchor;
            const float forward = GetPseudoFPBoneForwardOffset();
            const float side = GetPseudoFPBoneSideOffset();
            const float up = GetPseudoFPBoneUpOffset();

            if (g_SAFAnimationPlaying && g_HeadBone) {
                // SAF: get eye-level anchor from head anchor node/eye helpers,
                // fall back to passed headAnchor (neck bone) if unavailable
                RE::NiPoint3 safeAnchor = GetCurrentHeadAnchorWorldPosition();
                if (IsFinitePoint(safeAnchor)) {
                    worldPos = safeAnchor;
                }
                const auto& m = g_HeadBone->world.rotate;
                // Head-up Z tells us if the player is standing up or lying down:
                //   headUp.z > 0.5f  → upright (standing/kneeling)
                //   headUp.z <= 0.5f → prone/on back
                if (m.entry[2][2] > 0.5f) {
                    // Upright: use full head bone axes so offset follows head animation
                    // (forward = column 1, right = column 0, up = column 2)
                    worldPos.x += m.entry[0][1] * forward + m.entry[0][0] * side + m.entry[0][2] * up;
                    worldPos.y += m.entry[1][1] * forward + m.entry[1][0] * side + m.entry[1][2] * up;
                    worldPos.z += m.entry[2][1] * forward + m.entry[2][0] * side + m.entry[2][2] * up;
                } else {
                    // Prone/back: horizontal projection of head forward/right + world up
                    // so the camera doesn't move vertically when the head is tilted
                    RE::NiPoint3 horzFwd = { m.entry[0][1], m.entry[1][1], 0.0f };
                    RE::NiPoint3 horzRgt = { m.entry[0][0], m.entry[1][0], 0.0f };
                    bool fwdOk = NormalizePlanar(horzFwd);
                    bool rgtOk = NormalizePlanar(horzRgt);
                    if (fwdOk && rgtOk) {
                        worldPos.x += horzFwd.x * forward + horzRgt.x * side;
                        worldPos.y += horzFwd.y * forward + horzRgt.y * side;
                    } else {
                        float yaw = player ? player->GetAngleZ() : 0.0f;
                        RE::NiPoint3 f, r;
                        GetYawAxes(yaw, f, r);
                        worldPos.x += f.x * forward + r.x * side;
                        worldPos.y += f.y * forward + r.y * side;
                    }
                    worldPos.z += up;
                }
            } else if (cr && !g_SAFAnimationPlaying) {
                RE::NiPoint3 forwardAxis = {};
                RE::NiPoint3 rightAxis = {};
                float camYaw = std::atan2(cr->world.rotate.entry[1][0], cr->world.rotate.entry[0][0]);
                if (g_FurnitureYawLocked) {
                    constexpr float kMaxYawFurnitureRad = 90.0f * (3.14159265f / 180.0f);
                    camYaw = g_FurnitureBaseYaw + ClampFloat(WrapAnglePi(camYaw - g_FurnitureBaseYaw), -kMaxYawFurnitureRad, kMaxYawFurnitureRad);
                }
                GetYawAxes(camYaw, forwardAxis, rightAxis);
                worldPos.x += forwardAxis.x * forward + rightAxis.x * side;
                worldPos.y += forwardAxis.y * forward + rightAxis.y * side;
                worldPos.z += up;
            } else {
                RE::NiPoint3 forwardAxis = {};
                RE::NiPoint3 rightAxis = {};
                const float yaw = player ? player->GetAngleZ() : 0.0f;
                GetYawAxes(yaw, forwardAxis, rightAxis);
                worldPos.x += forwardAxis.x * forward + rightAxis.x * side;
                worldPos.y += forwardAxis.y * forward + rightAxis.y * side;
                worldPos.z += up;
            }
            return worldPos;
        }

        static RE::NiPoint3 GetNodeCenter(RE::NiAVObject* node)
        {
            if (!node) {
                return {};
            }
            if (node->worldBound.radius > 0.001f) {
                return node->worldBound.center;
            }
            return node->world.translate;
        }

        static RE::NiAVObject* FindPreferredEyeNode(RE::NiAVObject* root)
        {
            if (!root) {
                return nullptr;
            }

            static constexpr const char* kNames[] = {
                "Eye_Target",
                "faceBone_C_EyesFat"
            };

            for (auto* name : kNames) {
                auto* node = root->GetObjectByName(RE::BSFixedString(name));
                if (node) {
                    return node;
                }
            }

            return nullptr;
        }

        static bool IsPlayerUsingFurniture(RE::PlayerCharacter* player)
        {
            if (!player) {
                return false;
            }
            auto* proc = player->currentProcess;
            if (!proc || !proc->middleHigh) {
                return false;
            }
            return static_cast<bool>(proc->middleHigh->occupiedFurniture) || static_cast<bool>(proc->middleHigh->currentFurniture);
        }

        static RE::NiPoint3 ClampWorldPosToPlayerCapsule(RE::PlayerCharacter* player, const RE::NiPoint3& desiredWorldPos, const RE::NiPoint3& headAnchor)
        {
            if (!player) {
                return desiredWorldPos;
            }

            const RE::NiPoint3 boundMin = player->GetBoundMin();
            const RE::NiPoint3 boundMax = player->GetBoundMax();

            const float playerX = player->GetPositionX();
            const float playerY = player->GetPositionY();
            const float playerZ = player->GetPositionZ();

            float radiusX = (std::max)(std::fabs(boundMin.x - playerX), std::fabs(boundMax.x - playerX));
            float radiusY = (std::max)(std::fabs(boundMin.y - playerY), std::fabs(boundMax.y - playerY));
            float capsuleRadius = (std::max)(radiusX, radiusY);
            if (capsuleRadius < 0.20f || capsuleRadius > 1.00f) {
                capsuleRadius = 0.35f;
            }

            RE::NiPoint3 clamped = desiredWorldPos;
            float offsetX = desiredWorldPos.x - playerX;
            float offsetY = desiredWorldPos.y - playerY;
            float planarLen = std::sqrt(offsetX * offsetX + offsetY * offsetY);
            if (planarLen > capsuleRadius && planarLen > 0.0001f) {
                float scale = capsuleRadius / planarLen;
                clamped.x = playerX + offsetX * scale;
                clamped.y = playerY + offsetY * scale;
            }

            float minZ = boundMin.z;
            float maxZ = boundMax.z;
            if ((maxZ - minZ) < 0.5f || (maxZ - minZ) > 3.5f) {
                minZ = playerZ + 0.20f;
                maxZ = playerZ + (std::max)(g_EyeHeight + 0.10f, 1.20f);
            }

            maxZ = (std::max)(maxZ, headAnchor.z);
            clamped.z = ClampFloat(clamped.z, minZ, maxZ);
            return clamped;
        }

        static RE::NiPoint3 GetHeadMeshCenter(RE::NiAVObject* headMesh)
        {
            if (!headMesh) {
                return {};
            }

            if (headMesh->worldBound.radius > 0.001f) {
                return headMesh->worldBound.center;
            }

            return headMesh->world.translate;
        }

        static RE::NiPoint3 GetCurrentHeadAnchorWorldPosition()
        {
            if (g_HeadAnchorNode) {
                if (g_HeadAnchorNode == g_HeadBone) {
                    return g_HeadBone->world.translate;
                }
                if (g_HeadMesh && g_HeadAnchorNode == g_HeadMesh) {
                    return GetHeadMeshCenter(g_HeadMesh);
                }

                // Eye helper nodes track the animated face more accurately than
                // reconstructing their position back from C_Head.
                const RE::NiPoint3 directAnchor = g_HeadAnchorNode->world.translate;
                if (IsFinitePoint(directAnchor)) {
                    return directAnchor;
                }
            }

            if (!g_HeadBone) {
                return {};
            }

            const RE::NiPoint3 localAnchor = TransformLocalToWorld(g_HeadBone->world.rotate, g_HeadAnchorLocalOffset);
            return g_HeadBone->world.translate + localAnchor;
        }

        static void UpdateHeadAnchorData()
        {
            g_HeadAnchorNode = nullptr;
            g_HeadAnchorLocalOffset = {};

            if (!g_HeadBone) {
                g_HeadMesh = nullptr;
                return;
            }

            g_HeadMesh = g_HeadBone->parent ? static_cast<RE::NiAVObject*>(g_HeadBone->parent) : g_HeadBone;
            auto* preferred = FindPreferredEyeNode(g_PrevSkeletonRoot);
            if (preferred) {
                g_HeadAnchorNode = preferred;
            } else {
                g_HeadAnchorNode = g_HeadBone->parent ? static_cast<RE::NiAVObject*>(g_HeadBone->parent) : g_HeadBone;
            }

            if (!g_HeadAnchorNode) {
                g_HeadAnchorNode = g_HeadBone;
            }

            RE::NiPoint3 targetCenter = preferred ? preferred->world.translate : GetHeadMeshCenter(g_HeadMesh);
            RE::NiPoint3 headToCenter = targetCenter - g_HeadBone->world.translate;
            g_HeadAnchorLocalOffset = TransformWorldToLocal(g_HeadBone->world.rotate, headToCenter);
        }

        static float GetPluginDirIniFloat(const char* section, const char* key, float defaultValue)
        {
            const std::string& iniPath = GetPluginDirIniPath();
            char buf[32];
            GetPrivateProfileStringA(section, key, "", buf, sizeof(buf), iniPath.c_str());
            if (strlen(buf) == 0)
                return defaultValue;
            return static_cast<float>(atof(buf));
        }

        static bool GetPluginDirIniBool(const char* section, const char* key, bool defaultValue)
        {
            const std::string& iniPath = GetPluginDirIniPath();
            char buf[32];
            GetPrivateProfileStringA(section, key, "", buf, sizeof(buf), iniPath.c_str());
            if (strlen(buf) == 0) {
                return defaultValue;
            }

            std::string value(buf);
            std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return value == "1" || value == "true" || value == "yes" || value == "on";
        }

        static void GetPseudoFPOffsets(float& outX, float& outY, float& outZ)
        {
            outX = GetPluginDirIniFloat("PseudoFP", "fOffsetX", 0.0f);
            outY = GetPluginDirIniFloat("PseudoFP", "fOffsetY", 0.0f);
            outZ = GetPluginDirIniFloat("PseudoFP", "fOffsetZ", kDefaultEyeUpOffset);
        }

        static bool IsUltraRigidPseudoFPEnabled()
        {
            return GetPluginDirIniBool("PseudoFP", "bUltraRigidHeadAttach", false);
        }

        static bool IsUltraRigidIgnoreSanityChecksEnabled()
        {
            return GetPluginDirIniBool("PseudoFP", "bUltraRigidIgnoreSanityChecks", false);
        }

        static uint32_t GetPseudoFPHeadCacheGraceFrames()
        {
            const float configured = GetPluginDirIniFloat("PseudoFP", "fHeadAttachGraceFrames", 45.0f);
            const float clamped = ClampFloat(configured, 0.0f, 240.0f);
            return static_cast<uint32_t>(clamped + 0.5f);
        }

        static float GetUltraRigidMaxPlanarOffset()
        {
            return ClampFloat(GetPluginDirIniFloat("PseudoFP", "fUltraRigidMaxPlanarOffset", 0.60f), 0.20f, 1.50f);
        }

        static float GetUltraRigidHeightTolerance()
        {
            return ClampFloat(GetPluginDirIniFloat("PseudoFP", "fUltraRigidHeightTolerance", 0.50f), 0.10f, 3.50f);
        }

        static float GetPseudoFPBoneForwardOffset()
        {
            const float legacyForward = GetPluginDirIniFloat("PseudoFP", "fNoseForward", kDefaultEyeForwardOffset) -
                                        GetPluginDirIniFloat("PseudoFP", "fOffsetX", 0.0f);
            return GetPluginDirIniFloat("PseudoFP", "fForwardOffset", legacyForward);
        }

        static float GetPseudoFPBoneSideOffset()
        {
            return GetPluginDirIniFloat("PseudoFP", "fSideOffset", GetPluginDirIniFloat("PseudoFP", "fOffsetY", 0.0f));
        }

        static float GetPseudoFPBoneUpOffset()
        {
            return GetPluginDirIniFloat("PseudoFP", "fUpOffset", GetPluginDirIniFloat("PseudoFP", "fOffsetZ", kDefaultEyeUpOffset));
        }

        static float GetPseudoFPMaxYawRad()
        {
            const float deg = GetPluginDirIniFloat("PseudoFP", "fMaxYawDegrees", 90.0f);
            const float clamped = ClampFloat(deg, 30.0f, 180.0f);
            return clamped * (3.14159265f / 180.0f);
        }

        static bool IsReasonableHeadAnchor(RE::PlayerCharacter* player, const RE::NiPoint3& anchorPos)
        {
            if (!player) {
                return false;
            }

            const float dx = anchorPos.x - player->GetPositionX();
            const float dy = anchorPos.y - player->GetPositionY();
            const float planarDist = std::sqrt(dx * dx + dy * dy);
            if (planarDist > GetUltraRigidMaxPlanarOffset()) {
                return false;
            }

            const float expectedHeight = g_EyeHeight;
            const float actualHeight = anchorPos.z - player->GetPositionZ();
            const float tolerance = GetUltraRigidHeightTolerance();
            return actualHeight >= (expectedHeight - tolerance) &&
                   actualHeight <= (expectedHeight + tolerance);
        }

        static bool IsPlausibleHeadAnchor(RE::PlayerCharacter* player, const RE::NiPoint3& anchorPos)
        {
            if (!player || !IsFinitePoint(anchorPos)) {
                return false;
            }

            const float dx = anchorPos.x - player->GetPositionX();
            const float dy = anchorPos.y - player->GetPositionY();
            const float planarDist = std::sqrt(dx * dx + dy * dy);
            if (planarDist > 3.50f) {
                return false;
            }

            const float actualHeight = anchorPos.z - player->GetPositionZ();
            return actualHeight >= 0.20f && actualHeight <= 3.50f;
        }
    }

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

    RE::NiCamera* FindNiCamera(RE::TESCamera* tesCam)
    {
        if (!tesCam || !tesCam->cameraRoot) return nullptr;
        auto* rootNode = tesCam->cameraRoot.get();
        if (!rootNode) return nullptr;
        uint32_t numChildren = (std::min)(rootNode->children.size(), 256u);
        auto* childData = rootNode->children.data();
        for (uint32_t i = 0; i < numChildren; i++) {
            auto& child = childData[i];
            if (!child) continue;
            auto* cam = starfield_cast<RE::NiCamera*>(child.get());
            if (cam) return cam;
        }
        return nullptr;
    }

    RE::NiAVObject* GetCachedHeadBone()
    {
        return g_HeadAnchorNode ? g_HeadAnchorNode : g_HeadBone;
    }

    static void InitEyeHeight()
    {
        if (g_HasEyeHeight) return;
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return;
        auto guard = player->loadedData.LockRead();
        auto* loaded = *guard;
        if (!loaded || !loaded->data3D.get()) return;
        g_PrevSkeletonRoot = loaded->data3D.get();
        g_HeadBone = g_PrevSkeletonRoot->GetObjectByName(RE::BSFixedString("C_Head"));
        if (g_HeadBone) {
            UpdateHeadAnchorData();
            RE::NiPoint3 eyeCenter = GetCurrentHeadAnchorWorldPosition();
            if (IsUltraRigidPseudoFPEnabled()) {
                eyeCenter = ComputeUltraRigidCameraWorldPosition(player, ComputeUltraRigidHeadAnchorWorldPosition(player));
            }
            g_EyeHeight = eyeCenter.z - player->GetPositionZ();
            g_HasEyeHeight = true;
            LogFormatted("Init: C_Head eyeHeight=%.2f", g_EyeHeight);
        }
    }

    static void RefreshHeadBone()
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return;
        auto guard = player->loadedData.LockRead();
        auto* loaded = *guard;
        if (!loaded || !loaded->data3D.get()) return;
        auto* root = loaded->data3D.get();
        // Log if skeleton root changed
        if (root != g_PrevSkeletonRoot) {
            LogFormatted("Skeleton root change: %p -> %p", (void*)g_PrevSkeletonRoot, (void*)root);
            g_PrevSkeletonRoot = root;
        }
        auto* newBone = g_PrevSkeletonRoot->GetObjectByName(RE::BSFixedString("C_Head"));
        if (newBone) {
            if (newBone != g_HeadBone) {
                LogFormatted("C_Head change: %p -> %p", (void*)g_HeadBone, (void*)newBone);
            }
            g_HeadBone = newBone;
            UpdateHeadAnchorData();
            g_HeadAnchorMissCount = 0;
        } else {
            // C_Head not in current skeleton — animation might be swapping skeletons.
            // Keep only the last valid cached world anchor for a short grace window.
            LogFormatted("C_Head NOT FOUND this frame, using cached head anchor if valid (was %p)", (void*)g_HeadBone);
            g_HeadBone = nullptr;
            g_HeadAnchorNode = nullptr;
            g_HeadMesh = nullptr;
            g_HeadAnchorLocalOffset = {};
            g_HeadAnchorMissCount++;
        }
    }

    bool ComputePseudoFPPWorldPosition(RE::TESCamera* tesCam, RE::NiPoint3& outWorldPos, RE::NiPoint3* outHeadAnchor, bool* outUsingFallback)
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player || !tesCam) {
            return false;
        }

        InitEyeHeight();

        auto* cr = tesCam->cameraRoot.get();
        if (!cr) {
            return false;
        }

        RefreshHeadBone();

        bool usingFallback = false;
        RE::NiPoint3 anchorPos;
        const bool ultraRigidHeadAttach = IsUltraRigidPseudoFPEnabled();
        const RE::NiPoint3 playerEyeFallback = {
            player->GetPositionX(),
            player->GetPositionY(),
            player->GetPositionZ() + g_EyeHeight
        };
        if (g_HeadBone) {
            if (ultraRigidHeadAttach) {
                RE::NiPoint3 directBoneAnchor = ComputeUltraRigidHeadAnchorWorldPosition(player);
                if (IsFinitePoint(directBoneAnchor)) {
                    anchorPos = directBoneAnchor;
                    g_LastValidHeadAnchorWorld = anchorPos;
                    g_HasLastValidHeadAnchorWorld = true;

                    outWorldPos = ComputeUltraRigidCameraWorldPosition(player, anchorPos, cr);

                    if (outHeadAnchor) {
                        *outHeadAnchor = anchorPos;
                    }
                    if (outUsingFallback) {
                        *outUsingFallback = false;
                    }
                    return true;
                }
            }

            // Prefer the live anchor node position so locomotion and jump
            // animations cannot drag the camera behind the animated face.
            RE::NiPoint3 headAnchorPos = GetCurrentHeadAnchorWorldPosition();

            // Pseudo camera should follow the real animated head every frame.
            // The old "stable anchor" and strict sanity gate caused the camera
            // to fall back toward the capsule during locomotion/lean states.
            RE::NiPoint3 directAnchorPos = headAnchorPos;
            const bool ignoreSanity = ultraRigidHeadAttach && IsUltraRigidIgnoreSanityChecksEnabled();
            const bool acceptDirectAnchor = IsFinitePoint(directAnchorPos) &&
                (ignoreSanity || (ultraRigidHeadAttach ? IsPlausibleHeadAnchor(player, directAnchorPos) : IsReasonableHeadAnchor(player, directAnchorPos)));

            if (acceptDirectAnchor) {
                anchorPos = directAnchorPos;
                g_LastValidHeadAnchorWorld = anchorPos;
                g_HasLastValidHeadAnchorWorld = true;
            } else if (g_HasLastValidHeadAnchorWorld &&
                       (ignoreSanity ? IsFinitePoint(g_LastValidHeadAnchorWorld) :
                                       (ultraRigidHeadAttach ? IsPlausibleHeadAnchor(player, g_LastValidHeadAnchorWorld) :
                                                               IsReasonableHeadAnchor(player, g_LastValidHeadAnchorWorld)))) {
                anchorPos = g_LastValidHeadAnchorWorld;
                LogFormatted("PFPP_REJECT_BAD_HEAD: direct=(%.2f,%.2f,%.2f) cached=(%.2f,%.2f,%.2f) player=(%.2f,%.2f,%.2f)",
                    directAnchorPos.x, directAnchorPos.y, directAnchorPos.z,
                    g_LastValidHeadAnchorWorld.x, g_LastValidHeadAnchorWorld.y, g_LastValidHeadAnchorWorld.z,
                    player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
            } else {
                usingFallback = true;
                anchorPos = playerEyeFallback;
                g_HasLastValidHeadAnchorWorld = false;
                LogFormatted("PFPP_REJECT_BAD_HEAD_FALLBACK: direct=(%.2f,%.2f,%.2f) player=(%.2f,%.2f,%.2f)",
                    directAnchorPos.x, directAnchorPos.y, directAnchorPos.z,
                    player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
            }
        } else {
            const uint32_t kHeadAnchorGraceFrames = GetPseudoFPHeadCacheGraceFrames();
            constexpr float kMaxCachedAnchorDistanceSq = 6.25f;  // 2.5 units
            const bool canUseCachedAnchor = ultraRigidHeadAttach ?
                (g_HasLastValidHeadAnchorWorld && g_HeadAnchorMissCount <= kHeadAnchorGraceFrames && IsFinitePoint(g_LastValidHeadAnchorWorld)) :
                (g_HasLastValidHeadAnchorWorld &&
                 g_HeadAnchorMissCount <= kHeadAnchorGraceFrames &&
                 DistanceSquared(g_LastValidHeadAnchorWorld, playerEyeFallback) <= kMaxCachedAnchorDistanceSq);

                if (canUseCachedAnchor) {
                    anchorPos = g_LastValidHeadAnchorWorld;
                    if (ultraRigidHeadAttach) {
                        outWorldPos = ComputeUltraRigidCameraWorldPosition(player, anchorPos, cr);
                        if (outHeadAnchor) {
                            *outHeadAnchor = anchorPos;
                        }
                        if (outUsingFallback) {
                            *outUsingFallback = false;
                        }
                        return true;
                    }
            } else {
                usingFallback = true;
                anchorPos = playerEyeFallback;
                g_HasLastValidHeadAnchorWorld = false;
            }
        }

        // Nose forward offset: push camera from head center toward the nose,
        // using the head anchor's facing direction (local Y in Gamebryo skeletons)
        const float noseForward = GetPluginDirIniFloat("PseudoFP", "fNoseForward", kDefaultEyeForwardOffset);
        RE::NiPoint3 forwardAxis = {};
        RE::NiPoint3 rightAxis = {};
        GetPseudoFPAxes(player, forwardAxis, rightAxis);
        if (std::fabs(noseForward) > 0.0001f) {
            anchorPos.x += forwardAxis.x * noseForward;
            anchorPos.y += forwardAxis.y * noseForward;
        }

        float offsetX = 0.0f;
        float offsetY = 0.0f;
        float offsetZ = 0.0f;
        GetPseudoFPOffsets(offsetX, offsetY, offsetZ);

        RE::NiPoint3 worldPos = anchorPos;

        // INI offsets are defined in camera/head space, not world axes:
        // X = backward/forward, Y = right/left, Z = height.
        worldPos.x += (-forwardAxis.x * offsetX) + (rightAxis.x * offsetY);
        worldPos.y += (-forwardAxis.y * offsetX) + (rightAxis.y * offsetY);
        worldPos.z += offsetZ;

        // DEBUG: raw world-space offset per axis to determine correct forward direction
        // Set in ImprovedCamera.ini [PseudoFP]:
        //   fDebugOffsetX = 50   moves along world X
        //   fDebugOffsetY = 50   moves along world Y
        //   fDebugOffsetZ = 50   moves along world Z
        const float debugX = GetPluginDirIniFloat("PseudoFP", "fDebugOffsetX", 0.0f);
        const float debugY = GetPluginDirIniFloat("PseudoFP", "fDebugOffsetY", 0.0f);
        const float debugZ = GetPluginDirIniFloat("PseudoFP", "fDebugOffsetZ", 0.0f);
        worldPos.x += debugX;
        worldPos.y += debugY;
        worldPos.z += debugZ;

        RE::NiPoint3 unclampedWorldPos = worldPos;
        if (!ultraRigidHeadAttach) {
            if (usingFallback) {
                worldPos = ClampWorldPosToPlayerCapsule(player, worldPos, anchorPos);
            }
        }

        if (outHeadAnchor) {
            *outHeadAnchor = anchorPos;
        }
        if (outUsingFallback) {
            *outUsingFallback = usingFallback;
        }

        outWorldPos = worldPos;
        if (!usingFallback && g_HeadAnchorMissCount > 0) {
            LogFormatted("PFPP_HEAD_CACHE: missCount=%u anchor=(%.2f,%.2f,%.2f) world=(%.2f,%.2f,%.2f)",
                g_HeadAnchorMissCount,
                anchorPos.x, anchorPos.y, anchorPos.z,
                worldPos.x, worldPos.y, worldPos.z);
        }
        if (std::fabs(unclampedWorldPos.x - worldPos.x) > 0.001f ||
            std::fabs(unclampedWorldPos.y - worldPos.y) > 0.001f ||
            std::fabs(unclampedWorldPos.z - worldPos.z) > 0.001f) {
            LogFormatted("PFPP_CAPSULE_CLAMP: unclamped=(%.2f,%.2f,%.2f) clamped=(%.2f,%.2f,%.2f) anchor=(%.2f,%.2f,%.2f)",
                unclampedWorldPos.x, unclampedWorldPos.y, unclampedWorldPos.z,
                worldPos.x, worldPos.y, worldPos.z,
                anchorPos.x, anchorPos.y, anchorPos.z);
        }
        return true;
    }

    static int g_FrameCount = 0;

    bool ApplyPseudoFPPRig(RE::TESCamera* tesCam, void* tpsThis)
    {
        auto* camera = RE::PlayerCamera::GetSingleton();
        // Log if called while not in TPP (new: we now force-fix regardless of state)
        if (camera && !camera->IsInThirdPerson()) {
            auto* tcam = static_cast<RE::TESCamera*>(camera);
            uint32_t currentIdx = 0xFF;
            for (uint32_t i = 0; i < RE::CameraState::kTotal; i++) {
                if (tcam->currentState == camera->cameraStates[i]) {
                    currentIdx = i;
                    break;
                }
            }
            LogFormatted("PFPP_APPLY_NON_TPP: state=%u tpsThis=%p", currentIdx, tpsThis);
        }

        auto* niCam = FindNiCamera(tesCam);
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player || !niCam || !tesCam) return false;
        auto* cr = tesCam->cameraRoot.get();
        if (!cr) return false;
        g_CameraRoot = cr;
        g_NiCamera = niCam;

        bool usingFallback = false;
        RE::NiPoint3 headAnchor;
        RE::NiPoint3 worldPos;
        if (!ComputePseudoFPPWorldPosition(tesCam, worldPos, &headAnchor, &usingFallback)) {
            return false;
        }

        // Player model yaw rotation: previously fully disabled because
        // setting the player's yaw directly to camera yaw each frame created
        // a positive feedback loop with the engine's camera orbit system
        // (rotating the player shifts the orbit center, which changes camera
        // yaw, which rotates the player again - violent spinning during
        // furniture, ladders, ADS). Re-enabling with two changes meant to
        // break that loop:
        //   1) Rate-limited turning (max degrees/frame) instead of an
        //      instant snap to camera yaw, so any one frame's correction is
        //      small enough that it shouldn't be able to drive the orbit
        //      center far enough to produce a large camera-yaw change in
        //      return.
        //   2) Still gated by this function only running while
        //      camera->IsInThirdPerson() is true (see the early return in
        //      DetourTPSUpdate) - kFurniture/ladders/etc. never reach here,
        //      so the engine's own body-orientation logic for those states
        //      is left completely untouched.
        // Set kEnableBodyFollowsCameraYaw to false below to fully revert to
        // the old "never touch body rotation" behavior if this still spins.
        constexpr bool kEnableBodyFollowsCameraYaw = true;
        constexpr float kMaxYawTurnPerFrameDeg = 6.0f;  // tune: lower = slower turn, less feedback risk
        float camYaw = std::atan2(cr->world.rotate.entry[1][0], cr->world.rotate.entry[0][0]);
        float bodyYaw = player->GetAngleZ();
        const bool isFurniture = IsPlayerUsingFurniture(player) || (camera && camera->QCameraEquals(RE::CameraState::kFurniture));
        g_RestoreRootRotation = false;

        if (isFurniture) {
            if (!g_FurnitureYawLocked) {
                g_FurnitureBaseYaw = bodyYaw;
                g_FurnitureYawLocked = true;
            }

            // Use orbit yaw (engine-updated cr->world.translate position after origUpdate)
            // as the camera yaw source.  During kFurniture the engine processes mouse input
            // through the orbit system (cr->world.translate), not cr->world.rotate.
            constexpr float kMaxYawFurnitureRad = 90.0f * (3.14159265f / 180.0f);
            float clampedYaw = g_FurnitureBaseYaw + ClampFloat(WrapAnglePi(g_FurnitureOrbitYaw - g_FurnitureBaseYaw), -kMaxYawFurnitureRad, kMaxYawFurnitureRad);

            // Build new rotation matrix from clamped yaw + pitch extracted from current matrix
            float pitch = 0.0f;
            {
                RE::NiPoint3 cFwd = {
                    cr->world.rotate.entry[0][1],
                    cr->world.rotate.entry[1][1],
                    cr->world.rotate.entry[2][1]
                };
                float horizLen = std::sqrt(cFwd.x * cFwd.x + cFwd.y * cFwd.y);
                if (horizLen > 0.001f) {
                    pitch = std::atan2(-cFwd.z, horizLen);
                }
            }
            const float cY = std::cos(clampedYaw);
            const float sY = std::sin(clampedYaw);
            const float cP = std::cos(pitch);
            const float sP = std::sin(pitch);
            RE::NiMatrix3 clampedRot(
                cY,  -sY * cP,  -sY * sP,  0.0f,
                sY,   cY * cP,   cY * sP,  0.0f,
                0.0f, -sP,       cP,        0.0f
            );
            cr->world.rotate = clampedRot;
            cr->previousWorld.rotate = clampedRot;
            if (cr->parent) {
                cr->local.rotate = cr->parent->world.rotate.Transpose() * clampedRot;
            } else {
                cr->local.rotate = clampedRot;
            }

            // Recompute position with clamped yaw
            RE::NiPoint3 clampedPos;
            if (ComputePseudoFPPWorldPosition(tesCam, clampedPos, nullptr, nullptr)) {
                worldPos = clampedPos;
            }
            g_RestoreRootRotation = true;
        } else {
            g_FurnitureYawLocked = false;
            if (kEnableBodyFollowsCameraYaw && camera && camera->IsInThirdPerson()) {
                float diff = WrapAnglePi(camYaw - bodyYaw);
                float maxYaw = GetPseudoFPMaxYawRad();
                if (std::fabs(diff) > maxYaw) {
                    float clamped = ClampFloat(diff, -maxYaw, maxYaw);
                    float correction = diff - clamped;
                    float maxStep = kMaxYawTurnPerFrameDeg * (3.14159265f / 180.0f);
                    float step = ClampFloat(correction, -maxStep, maxStep);
                    if (std::fabs(step) > 0.0001f) {
                        player->data.angle.z = bodyYaw + step;
                    }
                }
            }
        }

        RE::NiPoint3 rootLocal = ComputeNodeLocalFromWorld(cr, worldPos);
        cr->local.translate = rootLocal;
        cr->previousWorld.translate = worldPos;
        cr->world.translate = worldPos;

        niCam->local.translate = { 0.0f, 0.0f, 0.0f };
        niCam->previousWorld.translate = worldPos;
        niCam->world.translate = worldPos;

        g_PrevRootLocal = rootLocal;
        g_PrevRootWorld = worldPos;
        g_PrevRootLocalRot = cr->local.rotate;
        g_PrevRootWorldRot = cr->world.rotate;
        g_PrevRootPrevWorldRot = cr->previousWorld.rotate;
        g_PrevSetLocal = {};
        g_PrevSetWorld = worldPos;

        // Write to this+0x1A8 as float2
        if (tpsThis) {
            float* ptr = reinterpret_cast<float*>((uintptr_t)tpsThis + 0x1A8);
            ptr[0] = 0.0f;
            ptr[1] = 0.0f;
        }

        // Yaw limiter disabled — directly modifying camera rotation matrices
        // (even in RestorePseudoRig after scene-graph) flips the camera
        // because the engine's internal yaw/pitch state diverges from the
        // overwritten matrix, causing accumulation of weird rotations.

        g_FrameCount++;
        if ((g_FrameCount % 30) == 0) {
            auto guard = player->loadedData.LockRead();
            auto* loaded = *guard;
            RE::NiPoint3 rootPos = {};
            if (loaded && loaded->data3D.get())
                rootPos = loaded->data3D->world.translate;
            RE::NiPoint3 headW = {};
            RE::NiPoint3 headNode = {};
            if (g_HeadMesh)
                headW = GetHeadMeshCenter(g_HeadMesh);
            if (g_HeadAnchorNode)
                headNode = g_HeadAnchorNode->world.translate;
            LogFormatted("PFPP: frame=%d eyeH=%.2f player=(%.2f,%.2f,%.2f) root=(%.2f,%.2f,%.2f) headMesh=(%.2f,%.2f,%.2f) headNode=(%.2f,%.2f,%.2f) anchor=(%.2f,%.2f,%.2f) cam=(%.2f,%.2f,%.2f) world=(%.2f,%.2f,%.2f) boneValid=%d fallback=%d",
                g_FrameCount, g_EyeHeight,
                player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(),
                rootPos.x, rootPos.y, rootPos.z,
                headW.x, headW.y, headW.z,
                headNode.x, headNode.y, headNode.z,
                headAnchor.x, headAnchor.y, headAnchor.z,
                cr->world.translate.x, cr->world.translate.y, cr->world.translate.z,
                worldPos.x, worldPos.y, worldPos.z,
                g_HeadBone ? 1 : 0, usingFallback ? 1 : 0);
            LogFormatted("ROT: col0=(%.2f,%.2f,%.2f) col1=(%.2f,%.2f,%.2f) col2=(%.2f,%.2f,%.2f)",
                cr->world.rotate.entry[0][0], cr->world.rotate.entry[1][0], cr->world.rotate.entry[2][0],
                cr->world.rotate.entry[0][1], cr->world.rotate.entry[1][1], cr->world.rotate.entry[2][1],
                cr->world.rotate.entry[0][2], cr->world.rotate.entry[1][2], cr->world.rotate.entry[2][2]);
            // vector from camera to player (should be forward direction)
            LogFormatted("FWDCHECK: cam2player=(%.2f,%.2f,%.2f)",
                player->GetPositionX() - cr->world.translate.x,
                player->GetPositionY() - cr->world.translate.y,
                player->GetPositionZ() - cr->world.translate.z);
        }
        if (usingFallback) {
            LogFormatted("FALLBACK: frame=%d bone=%p player=(%.2f,%.2f,%.2f)",
                g_FrameCount, (void*)g_HeadBone,
                player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
        }
        return true;
    }

    // TESCamera::Update (slot 03, ID 430192)
    using UpdateFunc = void (*)(RE::TESCamera*);
    UpdateFunc origUpdate = nullptr;

    void DetourUpdate(RE::TESCamera* a_this)
    {
        auto* camera = RE::PlayerCamera::GetSingleton();
        const bool isPlayerCam = (static_cast<RE::TESCamera*>(camera) == a_this);

        if (isPlayerCam) {
            // Compute angular velocity for Z-blend in PseudoFPPWorldPosition
            auto* player = RE::PlayerCharacter::GetSingleton();
            if (player) {
                static float g_LastPlayerAngleZ = 0.0f;
                float angleDelta = std::fabs(player->data.angle.z - g_LastPlayerAngleZ);
                g_LastPlayerAngleZ = player->data.angle.z;
                constexpr float kTurnThreshold = 0.0005f;
                constexpr float kTurnMax = 0.02f;
                if (angleDelta > kTurnThreshold) {
                    g_AngleBlendZ = std::min((angleDelta - kTurnThreshold) / (kTurnMax - kTurnThreshold), 1.0f);
                } else {
                    g_AngleBlendZ = 0.0f;
                }
            }
        }

        // Set camera pointers BEFORE origUpdate so that vtable hooks
        // (UWD/UTB/UT) inside the scene-graph traversal can restore
        // the camera position via RestorePseudoRig.
        if (isPlayerCam && g_PseudoFPPActive && (camera->IsInThirdPerson() || camera->QCameraEquals(RE::CameraState::kFurniture))) {
            auto* niCam = FindNiCamera(a_this);
            if (niCam) {
                g_NiCamera = niCam;
                g_CameraRoot = a_this->cameraRoot.get();
            }
        }

        origUpdate(a_this);

        if (!isPlayerCam) return;
        if (!g_PseudoFPPActive) return;
        if (!camera->IsInThirdPerson()) {
            auto* player = RE::PlayerCharacter::GetSingleton();
            bool safPlaying = player && SAFIntegration::IsSAFAnimationPlaying(player);
            const bool isFurniture = camera->QCameraEquals(RE::CameraState::kFurniture);

            // Furniture exit: reset camera to player position so the engine
            // has a clean base, preventing the player from clipping through
            // walls/ending up under the map on stand-up.
            if (!isFurniture && g_FurnitureYawLocked) {
                ResetCameraNodesToPlayer();
                g_FurnitureYawLocked = false;
                if (player) {
                    float camYaw = std::atan2(a_this->cameraRoot.get()->world.rotate.entry[1][0], a_this->cameraRoot.get()->world.rotate.entry[0][0]);
                    player->data.angle.z = camYaw;
                }
            }

            if (isFurniture) {
                auto* niCam = FindNiCamera(a_this);
                if (niCam) {
                    g_NiCamera = niCam;
                    g_CameraRoot = a_this->cameraRoot.get();
                }
                InitEyeHeight();
                if (!g_HasEyeHeight) {
                    return;
                }
                HideHead(true);
                {
                    // Save orbit yaw from engine-updated cr->world.translate (origUpdate set it)
                    // before ApplyPseudoFPPRig overrides it with head-level position.
                    auto* ply = RE::PlayerCharacter::GetSingleton();
                    auto* cr = a_this->cameraRoot.get();
                    if (ply && cr) {
                        g_FurnitureOrbitYaw = std::atan2(
                            cr->world.translate.x - ply->GetPositionX(),
                            ply->GetPositionY() - cr->world.translate.y);
                    }
                }
                ApplyPseudoFPPRig(a_this, nullptr);
                return;
            }
            if (!safPlaying) {
                g_NiCamera = nullptr;
                g_CameraRoot = nullptr;
            }
            return;
        }

        InitEyeHeight();
        if (!g_HasEyeHeight) return;

        auto* niCam = FindNiCamera(a_this);
        if (niCam) {
            g_NiCamera = niCam;
            float dx = niCam->world.translate.x - g_PrevSetWorld.x;
            float dy = niCam->world.translate.y - g_PrevSetWorld.y;
            float dz = niCam->world.translate.z - g_PrevSetWorld.z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            if (dist > 0.01f) {
                LogFormatted("TESCam_UPDATE_OVERWRITE: tpsSet=(%.2f,%.2f,%.2f) tesCamNow=(%.2f,%.2f,%.2f) dist=%.3f",
                    g_PrevSetWorld.x, g_PrevSetWorld.y, g_PrevSetWorld.z,
                    niCam->world.translate.x, niCam->world.translate.y, niCam->world.translate.z,
                    dist);
            }
            ApplyPseudoFPPRig(a_this, nullptr);
        }
    }

    // TPS::Update (vtable slot 12)
    using TPSUpdateFunc = void (*)(void*);
    TPSUpdateFunc origTPSUpdate = nullptr;

    static void HideHead(bool a_hide)
    {
        if (!g_HeadMesh) return;
        if (a_hide) {
            if (!g_HasSavedHeadCull) {
                g_WasHeadAppCulled = (g_HeadMesh->flags & 1) != 0;
                g_HasSavedHeadCull = true;
            }
            g_HeadMesh->flags |= 1;
        } else {
            if (g_HasSavedHeadCull && !g_WasHeadAppCulled)
                g_HeadMesh->flags &= ~1ULL;
            g_HasSavedHeadCull = false;
        }
    }

    void RestoreCameraOrbit()
    {
        auto* camera = RE::PlayerCamera::GetSingleton();
        if (!camera) return;
        auto* tesCam = static_cast<RE::TESCamera*>(camera);
        auto* cr = tesCam->cameraRoot.get();
        if (!cr) return;
        cr->local.translate = ComputeNodeLocalFromWorld(cr, cr->world.translate);
        cr->previousWorld.translate = cr->world.translate;
        auto* niCam = FindNiCamera(tesCam);
        if (niCam) {
            niCam->local.translate = {0,0,0};
            niCam->previousWorld.translate = cr->world.translate;
            niCam->world.translate = cr->world.translate;
        }
    }

    void ResetPseudoFPPState(bool a_restoreOrbit)
    {
        if (a_restoreOrbit) {
            RestoreCameraOrbit();
            HideHead(false);
        }
        g_CameraRoot = nullptr;
        g_NiCamera = nullptr;
        g_PrevRootLocal = {};
        g_PrevRootWorld = {};
        g_PrevRootLocalRot = {};
        g_PrevRootWorldRot = {};
        g_PrevRootPrevWorldRot = {};
        g_RestoreRootRotation = false;
        g_FurnitureYawLocked = false;
        g_FurnitureBaseYaw = 0.0f;
        g_PrevSetLocal = {};
        g_PrevSetWorld = {};
        g_HeadAnchorMissCount = 0;
        g_HasLastValidHeadAnchorWorld = false;
        g_LastValidHeadAnchorWorld = {};
    }

    // For vehicle/ship states: fully reset camera nodes to player position
    // so the ship camera system has a clean base to work from.
    void ResetCameraNodesToPlayer()
    {
        auto* camera = RE::PlayerCamera::GetSingleton();
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!camera || !player) return;
        auto* tesCam = static_cast<RE::TESCamera*>(camera);
        auto* cr = tesCam->cameraRoot.get();
        if (!cr) return;

        RE::NiPoint3 playerPos = {
            player->GetPositionX(),
            player->GetPositionY(),
            player->GetPositionZ()
        };

        cr->local.translate = ComputeNodeLocalFromWorld(cr, playerPos);
        cr->previousWorld.translate = playerPos;
        cr->world.translate = playerPos;

        auto* niCam = FindNiCamera(tesCam);
        if (niCam) {
            niCam->local.translate = {};
            niCam->previousWorld.translate = playerPos;
            niCam->world.translate = playerPos;
        }

        HideHead(false);
    }

    void ClearPseudoCameraPointers()
    {
        g_NiCamera = nullptr;
        g_CameraRoot = nullptr;
    }



    void DetourTPSUpdate(void* a_this)
    {
        auto* camera = RE::PlayerCamera::GetSingleton();
        auto* tesCam = camera ? static_cast<RE::TESCamera*>(camera) : nullptr;
        auto* niCam = tesCam ? FindNiCamera(tesCam) : nullptr;

        // Set camera pointers BEFORE origTPSUpdate so vtable hooks
        // inside the state-machine update can restore via RestorePseudoRig.
        // Only for TPP — non-TPP states must control the camera unaided.
        if (niCam && g_PseudoFPPActive && camera && camera->IsInThirdPerson()) {
            g_NiCamera = niCam;
            g_CameraRoot = tesCam->cameraRoot.get();
        }

        origTPSUpdate(a_this);

        if (!camera || !camera->IsInThirdPerson()) return;

        // Log what engine left NiCamera world at (recalculated from prev frame)
        if (g_PseudoFPPActive) {
            float dx = niCam->world.translate.x - g_PrevSetWorld.x;
            float dy = niCam->world.translate.y - g_PrevSetWorld.y;
            float dz = niCam->world.translate.z - g_PrevSetWorld.z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            if (dist > 0.01f) {
                LogFormatted("ENGINE_OVERWRITE: prevSet=(%.2f,%.2f,%.2f) engineNow=(%.2f,%.2f,%.2f) dist=%.3f",
                    g_PrevSetWorld.x, g_PrevSetWorld.y, g_PrevSetWorld.z,
                    niCam->world.translate.x, niCam->world.translate.y, niCam->world.translate.z,
                    dist);
            }
        }

        InitEyeHeight();
        if (!g_HasEyeHeight) return;

        if (!g_PseudoFPPActive) {
            HideHead(false);
            RestoreCameraOrbit();
            g_PrevRootLocal = {};
            g_PrevRootWorld = {};
            g_PrevSetLocal = {};
            g_PrevSetWorld = {};
            return;
        }

        HideHead(true);
        ApplyPseudoFPPRig(tesCam, a_this);
    }

    static int g_restoreCallCount = 0;
    static int g_restoreMatchCount = 0;
    static int g_restoreActionCount = 0;

    static void RestorePseudoRig(RE::NiAVObject* a_this, const char* stage)
    {
        if (!g_PseudoFPPActive) {
            return;
        }

        g_restoreCallCount++;

        const bool isTrackedObject = ((a_this == g_NiCamera && g_NiCamera) || (a_this == g_CameraRoot && g_CameraRoot));
        if (!isTrackedObject) {
            return;
        }

        if (g_SAFAnimationPlaying) {
            // SAF: use cached values from ApplyPseudoFPPRig (backup logic)
            // to avoid inconsistent head-bone position during scene-graph traversal.
            RE::NiPoint3 currentWorld = a_this->world.translate;
            float dx = currentWorld.x - g_PrevSetWorld.x;
            float dy = currentWorld.y - g_PrevSetWorld.y;
            float dz = currentWorld.z - g_PrevSetWorld.z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            const bool ultraRigid = IsUltraRigidPseudoFPEnabled();
            const float restoreThreshold = ultraRigid ? 0.001f : 0.01f;
            if (dist <= restoreThreshold) {
                if ((g_restoreCallCount % 600) == 0) {
                    LogFormatted("%s: obj=%p no-change (%.2f,%.2f,%.2f) prev=(%.2f,%.2f,%.2f) calls=%d match=%d act=%d",
                        stage, a_this,
                        currentWorld.x, currentWorld.y, currentWorld.z,
                        g_PrevSetWorld.x, g_PrevSetWorld.y, g_PrevSetWorld.z,
                        g_restoreCallCount, g_restoreMatchCount, g_restoreActionCount);
                }
                return;
            }
            g_restoreMatchCount++;
            g_restoreActionCount++;
            LogFormatted("%s: obj=%p propagation set world=(%.2f,%.2f,%.2f) restoring rootLocal=(%.2f,%.2f,%.2f) camLocal=(%.2f,%.2f,%.2f) world=(%.2f,%.2f,%.2f) calls=%d match=%d act=%d",
                stage, a_this,
                currentWorld.x, currentWorld.y, currentWorld.z,
                g_PrevRootLocal.x, g_PrevRootLocal.y, g_PrevRootLocal.z,
                g_PrevSetLocal.x, g_PrevSetLocal.y, g_PrevSetLocal.z,
                g_PrevSetWorld.x, g_PrevSetWorld.y, g_PrevSetWorld.z,
                g_restoreCallCount, g_restoreMatchCount, g_restoreActionCount);
            if (g_CameraRoot) {
                g_CameraRoot->local.translate = g_PrevRootLocal;
                g_CameraRoot->previousWorld.translate = g_PrevRootWorld;
                g_CameraRoot->world.translate = g_PrevRootWorld;
                if (g_RestoreRootRotation) {
                    g_CameraRoot->local.rotate = g_PrevRootLocalRot;
                    g_CameraRoot->previousWorld.rotate = g_PrevRootPrevWorldRot;
                    g_CameraRoot->world.rotate = g_PrevRootWorldRot;
                }
            }
            if (g_NiCamera) {
                g_NiCamera->local.translate = g_PrevSetLocal;
                g_NiCamera->previousWorld.translate = g_PrevSetWorld;
                g_NiCamera->world.translate = g_PrevSetWorld;
            }
        } else {
            // Vanilla: compute fresh camera position from current head bone
            // so even when this fires before ApplyPseudoFPPRig has run for
            // this frame, we get correct current-frame position, not stale.
            RE::NiPoint3 freshWorld;
            bool usingFallback = false;
            auto* camera = RE::PlayerCamera::GetSingleton();
            auto* tesCam = camera ? static_cast<RE::TESCamera*>(camera) : nullptr;
            if (!tesCam || !ComputePseudoFPPWorldPosition(tesCam, freshWorld, nullptr, &usingFallback)) {
                return;
            }
            RE::NiPoint3 currentWorld = a_this->world.translate;
            float dx = currentWorld.x - freshWorld.x;
            float dy = currentWorld.y - freshWorld.y;
            float dz = currentWorld.z - freshWorld.z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            const float restoreThreshold = 0.001f;
            if (dist <= restoreThreshold) {
                return;
            }
            g_restoreActionCount++;
            if (g_CameraRoot) {
                g_CameraRoot->local.translate = ComputeNodeLocalFromWorld(g_CameraRoot, freshWorld);
                g_CameraRoot->previousWorld.translate = freshWorld;
                g_CameraRoot->world.translate = freshWorld;
                if (g_RestoreRootRotation) {
                    g_CameraRoot->local.rotate = g_PrevRootLocalRot;
                    g_CameraRoot->previousWorld.rotate = g_PrevRootPrevWorldRot;
                    g_CameraRoot->world.rotate = g_PrevRootWorldRot;
                }
            }
            if (g_NiCamera) {
                g_NiCamera->local.translate = {};
                g_NiCamera->previousWorld.translate = freshWorld;
                g_NiCamera->world.translate = freshWorld;
            }
        }
    }

    static void* DetourUpdateWorldData(RE::NiAVObject* a_this, RE::NiUpdateData* a_data)
    {
        typedef void* (*UpdateWorldDataFunc)(RE::NiAVObject*, RE::NiUpdateData*);
        auto result = ((UpdateWorldDataFunc)g_origUpdateWorldData)(a_this, a_data);
        RestorePseudoRig(a_this, "UWD_BLOCK");
        return result;
    }

    static void* DetourUpdateTransformAndBounds(RE::NiAVObject* a_this, RE::NiUpdateData* a_data)
    {
        typedef void* (*UpdateTransformAndBoundsFunc)(RE::NiAVObject*, RE::NiUpdateData*);
        auto result = ((UpdateTransformAndBoundsFunc)g_origUpdateTransformAndBounds)(a_this, a_data);
        RestorePseudoRig(a_this, "UTB_BLOCK");
        return result;
    }

    static void* DetourUpdateTransforms(RE::NiAVObject* a_this, RE::NiUpdateData* a_data)
    {
        typedef void* (*UpdateTransformsFunc)(RE::NiAVObject*, RE::NiUpdateData*);
        auto result = ((UpdateTransformsFunc)g_origUpdateTransforms)(a_this, a_data);
        RestorePseudoRig(a_this, "UT_BLOCK");
        return result;
    }

    Hooks::Hooks() {}

    bool Hooks::Setup()
    {
        if (m_Setup)
            return true;
        if (MH_Initialize() != MH_OK)
            return false;

        auto vtabAddr = REL::ID(430192).address();
        auto funcAddr = *reinterpret_cast<void**>(vtabAddr + 3 * 8);
        if (funcAddr) {
            MH_CreateHook(funcAddr, (void*)DetourUpdate, (void**)&origUpdate);
            MH_EnableHook(funcAddr);
        }

        auto tpsVtabAddr = RE::VTABLE::ThirdPersonState[0].address();
        auto tpsFuncAddr = *reinterpret_cast<void**>(tpsVtabAddr + 12 * 8);
        if (tpsFuncAddr) {
            MH_CreateHook(tpsFuncAddr, (void*)DetourTPSUpdate, (void**)&origTPSUpdate);
            MH_EnableHook(tpsFuncAddr);
        }

        // Hook NiAVObject vtable to restore camera position after scene-graph
        // propagation overwrites it. Indices 78/79/80 match the original
        // SkyrimSE layout and are confirmed working for Starfield.
        auto avObjVtabAddr = RE::VTABLE::NiAVObject[0].address();
        auto uwFuncAddr = *reinterpret_cast<void**>(avObjVtabAddr + 78 * 8);
        if (uwFuncAddr) {
            MH_CreateHook(uwFuncAddr, (void*)DetourUpdateWorldData, (void**)&g_origUpdateWorldData);
            MH_EnableHook(uwFuncAddr);
            LogFormatted("UpdateWorldData hooked at %p (vtable %p, index 78)", uwFuncAddr, (void*)avObjVtabAddr);
        } else {
            Log("ERROR: Could not find UpdateWorldData function address");
        }

        auto utbFuncAddr = *reinterpret_cast<void**>(avObjVtabAddr + 79 * 8);
        if (utbFuncAddr) {
            MH_CreateHook(utbFuncAddr, (void*)DetourUpdateTransformAndBounds, (void**)&g_origUpdateTransformAndBounds);
            MH_EnableHook(utbFuncAddr);
            LogFormatted("UpdateTransformAndBounds hooked at %p (vtable %p, index 79)", utbFuncAddr, (void*)avObjVtabAddr);
        } else {
            Log("ERROR: Could not find UpdateTransformAndBounds function address");
        }

        auto utFuncAddr = *reinterpret_cast<void**>(avObjVtabAddr + 80 * 8);
        if (utFuncAddr) {
            MH_CreateHook(utFuncAddr, (void*)DetourUpdateTransforms, (void**)&g_origUpdateTransforms);
            MH_EnableHook(utFuncAddr);
            LogFormatted("UpdateTransforms hooked at %p (vtable %p, index 80)", utFuncAddr, (void*)avObjVtabAddr);
        } else {
            Log("ERROR: Could not find UpdateTransforms function address");
        }

        m_Setup = true;
        return true;
    }

    bool Hooks::Remove()
    {
        if (!m_Setup)
            return true;
        MH_Uninitialize();
        m_Setup = false;
        return true;
    }
}
