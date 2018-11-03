#pragma once

#include <Incarnate/Core/Math.hpp>
#include <Incarnate/Core/Error.hpp>
#include <Incarnate/Core/Buffer.hpp>
#include <Incarnate/Scene.hpp>
#include <Incarnate/Accelerator.hpp>
#include <Incarnate/Ray.hpp>
#include <Incarnate/FrameBuffer.hpp>
#include <Incarnate/Camera.hpp>

#include <array>

namespace Inc
{

class Renderer
{
public:
	void genCameraRays(int2 frame_size, size_t num_works)
	{
		const auto& camera = camera_[0];
		float fov_scale = std::tan(camera.fov*0.5f);

		#pragma omp parallel for num_threads(num_works)
		for(size_t i=0;i<path_states_.size();i++) {
			auto& s = path_states_[i];

			size_t pixel_id = i;
			float x = pixel_id%frame_size.x;
			float y = pixel_id/frame_size.x;
			float sx = 2.0f*(x/frame_size.x-0.5f);///screen-space x
			float sy = -2.0*(y/frame_size.y-0.5f);///screen-space y

			s.rayhit.ray.orig = camera.pos;
			s.rayhit.ray.dir = normalize(camera.dir + camera.up*sy*fov_scale + camera.right*sx*camera.aspect_ratio*fov_scale);
			s.phase = PathPhase::SceneIntersection;
			s.rayhit.hit.t = -1;
		}
	}

	void setup(FrameBuffer& frame_buffer, size_t num_works)
	{
		path_states_.reallocate(frame_buffer.size());
		for(size_t i=0;i<path_states_.size();i++)
			path_states_[i].phase = PathPhase::GenerateCameraRay;
		camera_.reallocate(1);
		frame_buffer_ = &frame_buffer;
	}

	virtual void render(FrameBuffer& frame_buffer, Scene* scene, const PerspectiveCamera& camera) = 0;

	void setAccelerator(Accelerator* accelerator)
	{
		accelerator_ = accelerator;
	}

protected:
	Buffer<PathState> path_states_;
	Buffer<PerspectiveCamera> camera_;
	FrameBuffer* frame_buffer_ = nullptr;
	size_t num_works_ = 0;
	Accelerator* accelerator_ = nullptr;
};

class NormalRenderer : public Renderer
{
public:
	void render(FrameBuffer& frame_buffer, Scene* scene, const PerspectiveCamera& camera) override;
};

class PathRenderer : public Renderer
{
public:
	void render(FrameBuffer& frame_buffer, Scene* scene, const PerspectiveCamera& camera) override;
};


}