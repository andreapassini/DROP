#pragma once

#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "DROP/ECS/beecs.h"
#include "DROP/math/transform.h"

using namespace bseecs;

constexpr EntityID ROOT_ID = -1;

class SceneGraphNode {
public:
	//uint32_t m_Id; // this will be the ECS id so we dont need it
	
	VgMath::Transform m_LocalTransform;
	VgMath::Transform m_CumulatedTransform; // cumulated transform

	// -1 for is in world space
	EntityID m_Parent = ROOT_ID;

	SceneGraphNode() 
		: m_Parent(-1), m_LocalTransform() { }

	SceneGraphNode(const VgMath::Transform& transform_val) 
		: m_Parent(-1), m_LocalTransform(transform_val) { };

	SceneGraphNode(const VgMath::Transform& transform_val, EntityID parent_val)
		: m_Parent(parent_val), m_LocalTransform(transform_val) { };

	~SceneGraphNode() {};

	void operator=(const SceneGraphNode& n) {
		// this mith be a mess, better fix it later
		this->m_Parent = n.m_Parent;
		this->m_LocalTransform = n.m_LocalTransform;
		this->m_CumulatedTransform = n.m_CumulatedTransform;
	}
};

namespace SceneGraph {
	void MoveNode(
		ECS* const ecs
		, const EntityID toMove
		, const EntityID newParent = ROOT_ID
	);
	void CalculateWorldTransforms(
		ECS* const ecs
	);
	void TransformToMatrix(
		const VgMath::Transform& inTransform
		, glm::mat4& outModelMatrix
	);

	void CalculateCumulatedTransform(
		ECS* const ecs
		, SceneGraphNode& node
	);
};

