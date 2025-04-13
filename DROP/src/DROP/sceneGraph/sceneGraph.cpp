#include "sceneGraph.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>
#include <future> 

#include <DROP/utils/Log.h>

// Calculate the cumulated transform starting from a local transform
// it also calculated the model matrix.
// It was not working with const ref
void SceneGraph::CalculateCumulatedTransform(
	ECS& ecs
	, TransformComponent& node
) {
	TransformComponent* currNode = &node;
	VgMath::Transform cumulated = currNode->m_LocalTransform;

	// Avoid recursion, saving stack
	while (currNode->m_Parent != -1) {
		cumulated = cumulated * currNode->m_LocalTransform;
		currNode = ecs.GetComponentPool<TransformComponent>().Get(currNode->m_Parent);
		if (!currNode) { break; }
	}

	node.m_CumulatedTransform = cumulated;
}

//void SceneGraph::MoveNode(
//	ECS* const ecs
//	, const EntityID toMove
//	, const EntityID newParent
//) {
//	SparseSet<SceneGraphNode>& compPool = ecs->GetComponentPool<SceneGraphNode>();
//	SceneGraphNode* node = compPool.Get(toMove);
//	if (!node) { LOG_CORE_ERROR("Node {0} not found", toMove);  return; }
//
//	SceneGraphNode* nodeParent = compPool.Get(node->m_Parent);
//	if (!nodeParent) { LOG_CORE_ERROR("NodeParent {0} not found", node->m_Parent); return; }
//
//	// recalculate the local transform so the cumulated is the same
//	VgMath::Transform parentLocalTransform = nodeParent->m_LocalTransform;
//	node->m_LocalTransform = node->m_LocalTransform * parentLocalTransform;
//		
//	node->m_Parent = newParent;
//}
//
//void SceneGraph::RemoveNode(
//	ECS* const ecs
//	, const EntityID toRemove
//) {
//	SparseSet<SceneGraphNode>& compPool = ecs->GetComponentPool<SceneGraphNode>();
//	SceneGraphNode* nodeToRemove = compPool.Get(toRemove);
//	if (!nodeToRemove) { LOG_CORE_ERROR("Node {0} not found", toRemove);  return; }
//
//	// Move every leaf into the parent
//	// "Get the those kids a step father."
//	std::vector<SceneGraphNode>& densePool = compPool.Data();
//	for (int32_t i = 0; i < densePool.size(); i++) {
//		auto& sceneGraphNode = densePool[i];
//		if (sceneGraphNode.m_Parent == toRemove) {
//			MoveNode(
//				ecs
//				, compPool.GetEntity(i)
//				, nodeToRemove->m_Parent
//			);
//		}
//	}
//
//	ecs->Remove<SceneGraphNode>(toRemove);
//}

// For each node of the graph, calculate the world transform
//	Is parallel using futures 
void SceneGraph::CalculateWorldTransforms(
	ECS& ecs
) {
	std::vector<TransformComponent>& denseCompPool = ecs.GetComponentPool<TransformComponent>().Data();

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
