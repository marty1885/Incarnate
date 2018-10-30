#pragma once

#include <Incarnate/Core/Math.hpp>
#include <Incarnate/Core/Error.hpp>
#include <Incarnate/Core/Buffer.hpp>
#include <Incarnate/Scene.hpp>
#include <Incarnate/Accelerator.hpp>
#include <Incarnate/Ray.hpp>
#include <Incarnate/FrameBuffer.hpp>

#include <stdint.h>

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

Ray generateCameraRay(const PerspectiveCamera& cam, float2 uv)
{
	Ray r;
	r.orig = cam.pos;
	return r;
}

class Renderer
{
public:
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
	Accelerator* accelerator_;
};

class NormalRenderer : public Renderer
{
public:


	void render(FrameBuffer& frame_buffer, Scene* scene, const PerspectiveCamera& camera)
	{
		camera_[0] = camera;
		//Stage 1, generate all camera rays
		genCameraRays((int2){frame_buffer.width(), frame_buffer.height()}, num_works_);

		accelerator_->intersect(path_states_);

		//Sage 2 fill with normal if hit
		float new_sample_weight = 1.f- (float)(frame_buffer.current_sample_)/(frame_buffer.current_sample_+1);
		#pragma omp parallel for num_threads(num_works_)
		for(size_t i=0;i<path_states_.size();i++) {
			auto& s = path_states_[i];
			float4 new_sample_color(0);
			if(s.t > 0) {
				float3 normal = float3(tirangle_normal(scene->meshes()[s.mesh_id]->triangle(s.geom_id)));
				new_sample_color = float4(abs(normal), 0);
			}
			else
				s.phase = PathPhase::Done;
			frame_buffer[i] = new_sample_weight*new_sample_color + (1.f-new_sample_weight)*frame_buffer[i];
		}
		frame_buffer.current_sample_ += 1;
	}

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

			s.orig = camera.pos;
			s.dir = normalize(camera.dir + camera.up*sy*fov_scale + camera.right*sx*camera.aspect_ratio*fov_scale);
			s.phase = PathPhase::SceneIntersection;
			s.t = -1;
		}
	}
};
}
