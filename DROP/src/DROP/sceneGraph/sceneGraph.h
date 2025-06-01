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
	VgMath::Transform localTransform;
	VgMath::Transform cumulatedTransform; // cumulated transform

	// -1 for is in world space
	EntityID parent = ROOT_ID;

	TransformComponent() 
		: parent(-1), localTransform() { }

	TransformComponent(const VgMath::Transform& transform_val) 
		: parent(-1), localTransform(transform_val) { };

	TransformComponent(const VgMath::Transform& transform_val, EntityID parent_val)
		: parent(parent_val), localTransform(transform_val) { };

	~TransformComponent() {};

	void operator=(const TransformComponent& n) {
		// this mith be a mess, better fix it later
		this->parent = n.parent;
		this->localTransform = n.localTransform;
		this->cumulatedTransform = n.cumulatedTransform;
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

