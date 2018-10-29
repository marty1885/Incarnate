#pragma once

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <Incarnate/Mesh.hpp>
#include <Incarnate/Ray.hpp>

namespace Inc
{

class Scene;

class Accelerator
{
public:
	virtual ~Accelerator() = default;

	virtual void addMesh(Mesh* mesh, const float4x4& transform_matrix) = 0;
	virtual void commitScene() = 0;
	virtual void intersect(Buffer<PathState>& rays) = 0;
};

class EmbreeAccelerator : public Accelerator
{
public:
	EmbreeAccelerator()
	{
		device_ = rtcNewDevice("threads=0");
		embree_scene_ = rtcNewScene(device_);
	}

	virtual ~EmbreeAccelerator()
	{
		rtcReleaseScene(embree_scene_);
		rtcReleaseDevice (device_);
	}

	void commitScene()
	{
		rtcCommitScene(embree_scene_);
		rtcInitIntersectContext(&context_);
	}

	virtual void addMesh(Mesh* mesh, const float4x4& transform_matrix) override;

	virtual void intersect(Buffer<PathState>& paths) override;


protected:
	RTCDevice device_;
	RTCScene embree_scene_;
	RTCIntersectContext context_;
};

}
