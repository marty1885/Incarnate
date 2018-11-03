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

inline float fastAcos(float x)
{
	float negate = float(x < 0);
	x = std::abs(x);
	float ret = -0.0187293f;
	ret = ret * x;
	ret = ret + 0.0742610f;
	ret = ret * x;
	ret = ret - 0.2121144f;
	ret = ret * x;
	ret = ret + 1.5707288f;
	ret = ret * std::sqrt(1.0-x);
	ret = ret - 2 * negate * ret;
	return negate * 3.14159265358979 + ret;
}

inline float fastAtan(float z)
{
	const float n1 = 0.97239411f;
	const float n2 = -0.19194795f;
	return (n1 + n2 * z * z) * z;
}


inline float fastAtan2(float y, float x)
{
	constexpr float PI = M_PI;
	constexpr float PI_2 = PI*0.5;
	if (x != 0.0f)
	{
		if(std::abs(x) > std::abs(y))
		{
			const float z = y / x;
			if(x > 0.f)
				return fastAtan(z);// atan2(y,x) = atan(y/x) if x > 0
			else if(y >= 0.f)
				
				return fastAtan(z) + PI;// atan2(y,x) = atan(y/x) + PI if x < 0, y >= 0
			else
				return fastAtan(z) - PI;// atan2(y,x) = atan(y/x) - PI if x < 0, y < 0
		}
		else // Use property atan(y/x) = PI/2 - atan(x/y) if |y/x| > 1.
		{
			const float z = x / y;
			if(y > 0.f)
				return -fastAtan(z) + PI_2;// atan2(y,x) = PI/2 - atan(x/y) if |y/x| > 1, y > 0
			else
				return -fastAtan(z) - PI_2;// atan2(y,x) = -PI/2 - atan(x/y) if |y/x| > 1, y < 0
		}
	}
	else
	{
		if(y > 0.f) // x = 0, y > 0
			return PI_2;
		else if(y < 0.f) // x = 0, y < 0
			return -PI_2;
	}
	return 0.0f; // x,y = 0. Could return NaN instead.
}

}
