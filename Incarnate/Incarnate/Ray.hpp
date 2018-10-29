#pragma once

#include <Incarnate/Core/Math.hpp>

namespace Inc
{

struct Ray
{
	float4 orig;
	float4 dir;
};

enum class PathPhase
{
	GenerateCameraRay = 0,
	SceneIntersection,
	Done,
};

struct PathState
{
	//Using float4 because most GPUs use float4 instead of float3
	float4 orig;
	float4 dir;
	float4 throughput;
	float4 ei;

	float4 p;
	float4 n;
	float tex_u, tex_v;
	PathPhase phase;

	float t;
	uint32_t mesh_id;
	uint32_t geom_id;
};

}
