#pragma once

#include <Incarnate/Core/Math.hpp>
#include <Incarnate/Core/Buffer.hpp>

namespace Inc
{

class Image
{
public:
	int width() const
	{
		return size_.x;
	}

	int height() const
	{
		return size_.y;
	}

	virtual bool good() = 0;
	virtual float4 getPixel(int2 coord) const = 0;

	int2 size_;
};

class HDRImage : public Image
{
	virtual bool load(const std::string& path, float gamma = 2.2f);

	virtual float4 getPixel(int2 coord) const override
	{
		return float4(0);
	}
	

protected:
	Buffer<float4> image_;
};

class LDRImage : public Image
{
};

}