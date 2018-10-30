#pragma once

#include <Incarnate/Core/Math.hpp>

namespace Inc
{

struct Ray
{
	//Using float4 because most GPUs use float4 instead of float3
	float4 orig;
	float4 dir;
};

struct Hit
{
	float tex_u, tex_v;
	float t;
	uint32_t mesh_id;
	uint32_t geom_id;
	uint32_t dummy__;
};

struct RayHit
{
	Ray ray;
	Hit hit;
};

enum class PathPhase
{
	GenerateCameraRay = 0,
	SceneIntersection,
	Done,
};

struct PathState
{
	RayHit rayhit;
	float4 throughput;
	float4 ei;

	float4 p;
	float4 n;
	PathPhase phase;
};

}
