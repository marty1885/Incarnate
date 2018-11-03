#include <Incarnate/Texture.hpp>

#include <Incarnate/3rdparty/stb_image.h>

using  namespace Inc;

bool HDRImage::load(const std::string& path, float gamma)
{
	int comp = 0;
	int width, height;
	float* image_data = stbi_loadf(path.c_str(), &width, &height, &comp, 4); //rogce RGBA

	image_.reallocate(width*height);

	if(image_data == nullptr) {
		std::cout << "Filed to load image " << path << std::endl;
		return false;
	}

	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++)
		{
			float4 pix = float4(image_data[4*(i*width+j)+0],image_data[4*(i*width+j)+1]
				,image_data[4*(i*width+j)+2],image_data[4*(i*width+j)+3]);

			//Flip the image verticaly. According to the OpenGL convention, the origin is at bottom-left.
			//But STBI assumes the origin is at the top-left.
			// /2.2 because stbi_loadf by default turns everything into linear rgb.
			image_[(height-i-1)*width+j] = glm::pow(pix,float4(gamma/2.2,gamma/2.2,gamma/2.2,1));
		}
	}

	stbi_image_free(image_data);
	return true;
}