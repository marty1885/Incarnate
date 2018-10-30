#pragma once

#include <unordered_map>

#include <Incarnate/Core/Math.hpp>
#include <Incarnate/Core/ComputeAPI.hpp>
#include <Incarnate/Core/Buffer.hpp>
#include <Incarnate/Accelerator.hpp>
#include <Incarnate/Mesh.hpp>

namespace Inc
{

struct Transformation
{
	void translate(float3 offset)
	{
		translation_matrix = glm::translate(offset);
		transform_cached = false;
	}

	void rotate(float3 axis, float angle)
	{
		rotation_matrix = glm::rotate(angle, axis);
		transform_cached = false;
	}

	void scale(float3 factor)
	{
		scaling_matrix = glm::scale(factor);
		transform_cached = false;
	}

	float4x4 transformMatrix() const
	{
		if(transform_cached == false) {
			transform_matrix = translation_matrix*rotation_matrix*scaling_matrix;
			transform_cached = true;
		}
		return transform_matrix;
	}

	float4x4 translation_matrix;
	float4x4 rotation_matrix;
	float4x4 scaling_matrix;

	mutable float4x4 transform_matrix;
	mutable bool transform_cached = false;
};

class SceneNode
{
public:
	virtual ~SceneNode() = default;

	void setTransform(Transformation transform)
	{
		transform_ = std::move(transform);
	}

	void translate(float3 offset)
	{
		transform_.translate(offset);
	}

	void rotate(float3 axis, float angle)
	{
		transform_.rotate(axis, angle);
	}

	void scale(float3 factor)
	{
		transform_.scale(factor);
	}

	float4x4 transformMatrix() const
	{
		return transform_.transformMatrix();
	}

	void addChild(SceneNode* node)
	{
		childs.push_back(node);
	}

	Transformation transform_;
	std::vector<SceneNode*> childs;
};

class SceneMeshNode : public SceneNode
{
public:
	SceneMeshNode(Mesh* mesh)
		: mesh_(mesh)
	{
	}

	SceneMeshNode() = default;

	Mesh* mesh()
	{
		return mesh_;
	}

	Mesh* mesh_;
};

class Scene
{
public:
	std::vector<SceneNode*> childs;

	void setAccelerator(Accelerator* accelerator)
	{
		accelerator_ = accelerator;
	}

	void add(SceneNode* item)
	{
		nodes_.push_back(item);
	}

	void commit()
	{
		for(auto node : nodes_)
			addToAccelerator(node);
		accelerator_->commitScene();
	}

	Mesh** meshes()
	{
		return meshes_.data();
	}

	std::vector<SceneNode*> nodes_;
	std::vector<Mesh*> meshes_;

	Accelerator* accelerator_ = nullptr;

protected:
	void addToAccelerator(SceneNode* current_node)
	{
		//TODO: Remove RTTI
		auto ptr = dynamic_cast<SceneMeshNode*>(current_node);
		if(ptr != nullptr) {
			int id = meshes_.size();
			meshes_.push_back(ptr->mesh());
			accelerator_->addMesh(ptr->mesh(), translate(float3(0,0,0)), id);
		}
		for(auto child : current_node->childs)
			addToAccelerator(child);
	}
};

}
