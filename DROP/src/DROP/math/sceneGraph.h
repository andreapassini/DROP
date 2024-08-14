#pragma once

#include "transform.h"
#include "vector3.h"
#include "versor3.h"
#include "mat3.h"

#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Node {
public:
	uint32_t m_Id;
	VgMath::Transform m_LocalTransform;
	Node* m_Parent;

	Node() 
		: m_Id(), m_Parent(nullptr), m_LocalTransform() { }

	Node(const uint32_t id_val) 
		: m_Id(id_val), m_Parent(nullptr), m_LocalTransform() { };

	Node(const uint32_t id_val, const VgMath::Transform transform_val) 
		: m_Id(id_val), m_Parent(nullptr), m_LocalTransform(transform_val) { };

	Node(const uint32_t id_val, const VgMath::Transform transform_val, Node* parent_val) 
		: m_Id(id_val), m_Parent(parent_val), m_LocalTransform(transform_val) { };

	~Node() {};

	void operator=(const Node& n) {
		this->m_Id = n.m_Id;
		this->m_Parent = n.m_Parent;
		this->m_LocalTransform = n.m_LocalTransform;
		//Node(n.id, n.localTransform, n.parent);
	}

	VgMath::Transform CalculateCumulativeTransform() const 
	{
		VgMath::Transform cumulativeTransform;
		cumulativeTransform = m_LocalTransform;

		if (this->m_Parent != nullptr) {	// Has a parent
			if(this->m_Parent->m_Id != 0){	// The parent is not the world
				cumulativeTransform = this->m_Parent->CalculateCumulativeTransform() * m_LocalTransform;
			}
		}

		return cumulativeTransform;
	}

private:

};

class SceneGraph {
public:
	SceneGraph(const uint32_t sizeEstimation);
	~SceneGraph() {};
	uint32_t AddObject(const uint32_t parentId);
	uint32_t AddObject(
		const uint32_t parentId, 
		const VgMath::Transform& transform
	);
	void DeleteNode(const uint32_t id_val);
	void CalculateWorldTransforms(
		std::unordered_map<uint32_t, 
		VgMath::Transform>& cumulatedTransforms
	);
	static void TransformToMatrix(
		const VgMath::Transform& inTransform, 
		glm::mat4& outModelMatrix
	);
public:
	// Wanna keep this so i can easily find the one i need and loop on through every node
	std::unordered_map<uint32_t, Node> m_GameObjects;
	static constexpr uint32_t ROOT_ID = 0;

private:
	static void CalculateSingleWorldTransform(
		const Node& node,
		VgMath::Transform* cumulatedTransform
	);
private:
	Node* m_World;
	uint32_t m_Index;
};

