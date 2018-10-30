#pragma once

#include <Incarnate/Core/Math.hpp>

namespace Inc
{

struct PerspectiveCamera
{
public:
	float4 pos;
	float4 dir;
	float4 up;
	float4 right;
	float fov = M_PI*.5f;
	float aspect_ratio = 1.0f;
};

}