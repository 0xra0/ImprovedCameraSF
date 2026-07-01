#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace IC::Math {

    inline float DegToRad(float degrees)
    {
        return degrees * (glm::pi<float>() / 180.0f);
    }

    inline float RadToDeg(float radians)
    {
        return radians * (180.0f / glm::pi<float>());
    }

    inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t)
    {
        return a + (b - a) * t;
    }

    inline float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    inline float Clamp(float value, float min, float max)
    {
        return std::clamp(value, min, max);
    }

    inline float SmoothStep(float t)
    {
        return t * t * (3.0f - 2.0f * t);
    }

    inline glm::quat Slerp(const glm::quat& a, const glm::quat& b, float t)
    {
        return glm::slerp(a, b, t);
    }

    inline glm::mat4 CreateViewMatrix(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
    {
        return glm::lookAtLH(position, position + forward, up);
    }

    inline glm::mat4 CreatePerspectiveMatrix(float fov, float aspect, float nearPlane, float farPlane)
    {
        return glm::perspectiveLH_ZO(fov, aspect, nearPlane, farPlane);
    }
}
