#include <Incarnate/Texture.hpp>

#include <Incarnate/3rdparty/stb_image.h>

using  namespace Inc;

bool HDRImage::load(const std::string& path, float gamma)
{
	int comp = 0;
	int width = 0, height = 0;

	//HACK: STB can only handle images up to 2GB after decompressing due to using usngeed 32bit integers
	//So the Alpha channel is not processed to let STB read 16K HDR files (otherwise a 16K HDR RGBA image is exactly 2GB)
	float* image_data = stbi_loadf(path.c_str(), &width, &height, &comp, 3); //rogce RGBA

	if(image_data == nullptr) {
		std::cout << "Filed to load image " << path << ". " << stbi_failure_reason() << std::endl;
		return false;
	}

	image_.reallocate(width*height);

	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			float4 pix = float4(image_data[3*(i*width+j)+0],image_data[3*(i*width+j)+1]
				,image_data[3*(i*width+j)+2],1);

			//Flip the image verticaly. According to the OpenGL convention, the origin is at bottom-left.
			//But STBI assumes the origin is at the top-left.
			// /2.2 because stbi_loadf by default turns everything into linear rgb.
			image_[(height-i-1)*width+j] = glm::pow(pix,float4(gamma/2.2,gamma/2.2,gamma/2.2,1));
		}
	}
	size_ = {width, height};

	stbi_image_free(image_data);
	return true;
}

float4 HDRImage::getPixel(int2 coord) const
{
	int x = clamp(coord.x, 0, width()-1);
	int y = clamp(coord.y, 0, height()-1);
	return image_[y*width()+x];
}

