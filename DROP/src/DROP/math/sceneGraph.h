#pragma once

#include "transform.h"
#include "vector3.h"
#include "versor3.h"
#include "mat3.h"

#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "../ECS/beecs.h"

using namespace bseecs;

class SceneGraphNode {
public:
	//uint32_t m_Id; // this will be the ECS id so we dont need it
	
	VgMath::Transform m_LocalTransform;
	VgMath::Transform m_CumulatedTransform; // cumulated transform
	
	//SceneGraphNode* m_Parent; //  this will be a stored ECS id
	EntityID m_Parent;

	SceneGraphNode() 
		: m_Parent(0), m_LocalTransform() { }

	SceneGraphNode(const VgMath::Transform& transform_val) 
		: m_Parent(0), m_LocalTransform(transform_val) { };

	SceneGraphNode(const VgMath::Transform& transform_val, EntityID parent_val)
		: m_Parent(parent_val), m_LocalTransform(transform_val) { };

	~SceneGraphNode() {};

	void operator=(const SceneGraphNode& n) {
		// this mith be a mess, better fix it later
		this->m_Parent = n.m_Parent;
		this->m_LocalTransform = n.m_LocalTransform;
		this->m_CumulatedTransform = n.m_CumulatedTransform;
	}

	SceneGraphNode* CalculateCumulativeTransform(ECS* const ecs)
	{
		if(m_Parent != 0){	// The parent is not the world
			SceneGraphNode* currentParentNode = ecs->GetComponentPool<SceneGraphNode>().Get(m_Parent);
			m_CumulatedTransform = currentParentNode->CalculateCumulativeTransform(ecs)->m_CumulatedTransform * m_LocalTransform;
		}
		else
		{
			m_CumulatedTransform = m_LocalTransform;
		}
	
		return this;
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
	std::unordered_map<uint32_t, SceneGraphNode> m_GameObjects;
	static constexpr uint32_t ROOT_ID = 0;

private:
	static void CalculateSingleWorldTransform(
		const SceneGraphNode& node,
		VgMath::Transform* cumulatedTransform
	);
private:
	SceneGraphNode* m_World;
	uint32_t m_Index;
};

