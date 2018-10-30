#pragma once

#include <Incarnate/Core/ComputeAPI.hpp>
#include <Incarnate/Core/Buffer.hpp>
#include <Incarnate/Core/Math.hpp>

#include <array>

namespace Inc
{

class SceneNode;

class Mesh
{
public:
	void setVertices(const float4* ptr, size_t num)
	{
		vertices_.reallocate(num);
		for(size_t i=0;i<num;i++)
			vertices_[i] = ptr[i];
	}

	void setIndices(const unsigned int* ptr, size_t num)
	{
		indices_.reallocate(num);
		for(size_t i=0;i<num;i++)
			indices_[i] = ptr[i];
	}

	const Buffer<float4>& vertices() const
	{
		return vertices_;
	}

	const Buffer<unsigned int>& indices() const
	{
		return indices_;
	}

	std::array<float4, 3> triangle(unsigned int index)
	{
		std::array<float4, 3> vertices;
		for(unsigned int i=0;i<3;i++)
			vertices[i] = vertices_[indices_[index*3+i]];
		return vertices;
	}

	Buffer<float4> vertices_;
	Buffer<unsigned int> indices_;
	Buffer<float4> normals_;
};
}
