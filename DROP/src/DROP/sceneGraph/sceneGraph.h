#pragma once

#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "DROP/ECS/beecs.h"
#include "DROP/math/transform.h"

using namespace bseecs;

constexpr bseecs::EntityID ROOT_ID = -1;

class TransformComponent {
public:
	VgMath::Transform m_LocalTransform;
	VgMath::Transform m_CumulatedTransform; // cumulated transform

	// -1 for is in world space
	EntityID m_Parent = ROOT_ID;

	TransformComponent() 
		: m_Parent(-1), m_LocalTransform() { }

	TransformComponent(const VgMath::Transform& transform_val) 
		: m_Parent(-1), m_LocalTransform(transform_val) { };

	TransformComponent(const VgMath::Transform& transform_val, EntityID parent_val)
		: m_Parent(parent_val), m_LocalTransform(transform_val) { };

	~TransformComponent() {};

	void operator=(const TransformComponent& n) {
		// this mith be a mess, better fix it later
		this->m_Parent = n.m_Parent;
		this->m_LocalTransform = n.m_LocalTransform;
		this->m_CumulatedTransform = n.m_CumulatedTransform;
	}
};

namespace SceneGraph {

	// #TODO
	// THIS 2 NEEDS to be reworked
	//void MoveNode(
	//	ECS& ecs
	//	, const EntityID toMove
	//	, const EntityID newParent = ROOT_ID
	//);
	//void RemoveNode(
	//	ECS& ecs
	//	, const EntityID toRemove
	//);

	// Calculate the cumulated Transform 
	// of each node of the graph
	void CalculateWorldTransforms(
		ECS& ecs
	);

	void TransformToMatrix(
		const VgMath::Transform& inTransform
		, glm::mat4& outModelMatrix
	);

	void CalculateCumulatedTransform(
		ECS& ecs
		, TransformComponent& node
	);
};

