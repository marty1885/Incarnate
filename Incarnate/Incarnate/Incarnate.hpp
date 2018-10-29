#pragma once

#include <Incarnate/Core/Math.hpp>
#include <Incarnate/Core/Error.hpp>
#include <Incarnate/Core/Buffer.hpp>
#include <Incarnate/Scene.hpp>
#include <Incarnate/Accelerator.hpp>
#include <Incarnate/Ray.hpp>

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

class FrameBuffer
{
public:
	FrameBuffer() = default;
	void create(size_t w, size_t h, size_t c=3)
	{
		width_ = w;
		height_ = h;
		channels_ = c;
		frame_buffer_.reallocate(w*h*c);
		clear();
	}

	bool defined() const
	{
		return frame_buffer_.size() != 0;
	}

	void clear()
	{
		current_sample_ = 0;
		auto data = frame_buffer_.data();
		for(size_t i=0;i < frame_buffer_.size();i++)
			data[i] = float4(0);
		//TODO: clear device data
	}

	int width() const
	{
		return width_;
	}

	int height() const
	{
		return height_;
	}

	int channels() const
	{
		return channels_;
	}

	float4* rawBuffer()
	{
		return frame_buffer_.data();
	}

	const float4* constBuffer() const
	{
		return frame_buffer_.data();
	}

	size_t size() const
	{
		return frame_buffer_.size();
	}

	inline float4& operator[] (size_t i)
	{
		return frame_buffer_[i];
	}

	inline const float4& operator[] (size_t i) const
	{
		return frame_buffer_[i];
	}

	size_t current_sample_ = 0;
protected:
	Buffer<float4> frame_buffer_;
	size_t width_;
	size_t height_;
	size_t channels_;
};

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

	void render(FrameBuffer& frame_buffer, Scene* scene, const PerspectiveCamera& camera)
	{
		camera_[0] = camera;
		//First, generate all camera rays
		genCameraRays((int2){frame_buffer.width(), frame_buffer.height()}, num_works_);

		accelerator_->intersect(path_states_);

		//TEST: fill the screen with ray dir
		//#pragma omp parallel for num_threads(num_works_)
		float new_sample_weight = 1.f- (float)(frame_buffer.current_sample_)/(frame_buffer.current_sample_+1);
		for(size_t i=0;i<path_states_.size();i++) {
			auto& s = path_states_[i];
			float4 new_sample_color(0);
			if(s.t > 0)
				new_sample_color = float4(abs(float3(s.dir)), 0);
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
}
