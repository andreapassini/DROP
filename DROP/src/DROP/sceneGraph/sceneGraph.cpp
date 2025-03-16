#include "sceneGraph.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>
#include <future> 

// Calculate the cumulated transform starting from a local transform
// it also calculated the model matrix.
// It was not working with const ref
void SceneGraph::CalculateCumulatedTransform(
	ECS* const ecs
	, SceneGraphNode& node
) {
	SceneGraphNode* currNode = &node;
	VgMath::Transform cumulated = currNode->m_LocalTransform;

	// Avoid recursion, saving stack
	while (currNode->m_Parent != -1) {
		cumulated = cumulated * currNode->m_LocalTransform;
		currNode = ecs->GetComponentPool<SceneGraphNode>().Get(currNode->m_Parent);
		if (!currNode) { break; }
	}

	node.m_CumulatedTransform = cumulated;
}

void SceneGraph::MoveNode(
	ECS* const ecs
	, const EntityID toMove
	, const EntityID newParent
) {
	SparseSet<SceneGraphNode>& compPool = ecs->GetComponentPool<SceneGraphNode>();
	SceneGraphNode* node = compPool.Get(toMove);
	if (node) {
		node->m_Parent = newParent;
	}
}

void SceneGraph::CalculateWorldTransforms(
	ECS* const ecs
) {
	std::vector<SceneGraphNode>& denseCompPool = ecs->GetComponentPool<SceneGraphNode>().Data();

	std::vector<std::future<void>> futures;

	for (auto& it : denseCompPool) {
		futures.push_back(
			std::async(std::launch::async, 
				CalculateCumulatedTransform
				, ecs
				, it
			)
		);		
	}

	for (auto& handle : futures) {
		handle.wait();
	}
}

// Transform converted into a glm model matrix
// This is not included directly in the transform file to avoid using an external math library there
// - outModelMatrix: must be initialized to identity matrix
void SceneGraph::TransformToMatrix(
	const VgMath::Transform& inTransform
	, glm::mat4& outModelMatrix
) {
	outModelMatrix = glm::translate(
		outModelMatrix,
		glm::vec3(
			inTransform.m_Translate.x,
			inTransform.m_Translate.y,
			inTransform.m_Translate.z
		)
	);

	outModelMatrix = glm::rotate(
		outModelMatrix,
		(float)(inTransform.m_Rotate.getAngleRadians()),
		glm::vec3(
			(float)(inTransform.m_Rotate.getAxis().x),
			(float)(inTransform.m_Rotate.getAxis().y),
			(float)(inTransform.m_Rotate.getAxis().z)
		)
	);

	outModelMatrix = glm::scale(
		outModelMatrix,
#if ANISOTROPIC_SCALING
		glm::vec3(
			(float)inTransform.scale.x,
			(float)inTransform.scale.y,
			(float)inTransform.scale.z
		)
#else
		glm::vec3(
			(float)inTransform.m_Scale,
			(float)inTransform.m_Scale,
			(float)inTransform.m_Scale
		)
#endif
	);
}
