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

	int2 dimenstion() const
	{
		return size_;
	}

	//virtual bool good() = 0;
	virtual float4 getPixel(int2 coord) const = 0;

	int2 size_;
};

class HDRImage : public Image
{
public:
	virtual bool load(const std::string& path, float gamma = 2.2f);

	virtual float4 getPixel(int2 coord) const override;

protected:
	Buffer<float4> image_;
};

class LDRImage : public Image
{
};

enum class TextureFilterMethod
{
	Nearest = 0,
	Bilinear
};

enum class UVWrapMode
{
	Clamp = 0,
	Repeat,
	MirroredRepeat
};

class Texture
{
public:
	float4 getPixel(float2 uv) const
	{
		uv = wrapUV(uv);
		float2 image_size = float2(image_->dimenstion());

		switch (filter_method_)
		{
			case TextureFilterMethod::Bilinear:
			{
				float2 texCentCoord = uv*image_size - float2(0.5);
				int x = texCentCoord.x;
				int y = texCentCoord.y;
				int2 texVexCoord = int2(texCentCoord);
				float2 texCoord = texCentCoord;
				float2 ratio = texCoord - float2(texVexCoord);
				float2 oppositeRatio = float2(1.f) - ratio;
				float4 pixels[4] = {image_->getPixel({x, y}), image_->getPixel({x+1,y})
					, image_->getPixel({x,y+1}), image_->getPixel({x+1,y+1})};
				float4 interpolate[2] = {pixels[0]*oppositeRatio.x + pixels[1]*ratio.x
					, pixels[2]*oppositeRatio.x + pixels[3]*ratio.x};
				float4 result = interpolate[0] * oppositeRatio.y
					+ interpolate[1] * ratio.y;
				return result;
				break;
			}

			case TextureFilterMethod::Nearest:
			default:
			{
				int2 coord = image_size*uv;
				return image_->getPixel(coord);
			}
		}
		//Should not execute the line of code
		return float4(0,0,0,1);
	}


	void setImage(Image* img)
	{
		image_ = img;
	}
	inline Image* getImage()
	{
		return image_;
	}

	inline float2 wrapUV(const float2& uv) const
	{
		switch (wrap_mode_)
		{
			case UVWrapMode::Repeat:
			{
				return glm::fract(uv);
				break;
			}
			case UVWrapMode::MirroredRepeat:
			{
				float2 vec = glm::fract(uv);
				int2 block = uv-vec;
				vec = float2((block.x%2==0?vec.x:1.f-vec.x),(block.y%2==0?vec.y:1.f-vec.y));
				return vec;
				break;
			}
			case UVWrapMode::Clamp:
			default:
			{
				return clamp(uv,float2(0,0),float2(0.99999f));
				break;
			}
		}
		return float2(0);
	}

	void setFilteringMethod(TextureFilterMethod method)
	{
		filter_method_ = method;
	}

	TextureFilterMethod getFilteringMethod() const
	{
		return filter_method_;
	}
protected:
	Image* image_ = nullptr;
	TextureFilterMethod filter_method_ = TextureFilterMethod::Bilinear;
	UVWrapMode wrap_mode_ = UVWrapMode::Repeat;
};


class EnvironmentMap
{
public:
	virtual float4 getPixel(float4 vec) const = 0;
protected:
};

class SphereMap : public EnvironmentMap
{
public:
	SphereMap(Texture* tex) : texture_(tex) {}
	float4 getPixel(float4 direction) const;
	void setTexture(Texture* tex);
	Texture* texture();
protected:
	Texture* texture_ = nullptr;
};

}