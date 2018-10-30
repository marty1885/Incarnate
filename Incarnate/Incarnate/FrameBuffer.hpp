#pragma once

#include <Incarnate/Core/Math.hpp>
#include <Incarnate/Core/Buffer.hpp>

namespace Inc
{

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

}
