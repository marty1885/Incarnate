#pragma once

#define GLM_FORCE_AVX //Remove this line if AVX is not avliable
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#define USE_MATH_DEFINES
#include <cmath>
#include <array>

namespace Inc
{
using float3 = glm::vec3;
using float4 = glm::vec4;
using float2 = glm::vec2;
using float4x4 = glm::mat4;
using int3 = glm::ivec3;

using int2 = glm::ivec2;

template<typename T>
T cross(T a, T b)
{
	return glm::cross(a, b);
}

template<typename T>
T dot(T a, T b)
{
	return glm::dot(a, b);
}

template<typename T>
T normalize(T a, T b)
{
	return glm::normalize(a, b);
}

template<typename T>
T clamp(T x, T left, T right)
{
	return glm::clamp(x, left, right);
}

inline float3 abs(float3 v)
{
	return glm::abs(v);
}

inline float4 abs(float4 v)
{
	return glm::abs(v);
}

inline float2 abs(float2 v)
{
	return glm::abs(v);
}

inline float4 triangleNormal(std::array<float4, 3> triangle)
{
	auto normal = glm::cross(float3(triangle[1]-triangle[0]), float3(triangle[2]-triangle[0]));
	return float4(normalize(normal), 0);
}

}
