#include "sceneGraph.h"
#include <glm/gtc/matrix_transform.hpp>

SceneGraph::SceneGraph(const uint32_t sizeEstimation)
{
	nodes.reserve(sizeEstimation);

	index = 0;
	Node rootNode(index, VgMath::Transform());	// Root or world always at index 0 

	nodes[index] = rootNode;
	world = &nodes[index];
}

uint32_t SceneGraph::AddNode(const uint32_t parentId_val)
{
	AddNode(parentId_val, VgMath::Transform());

	return index;
}

uint32_t SceneGraph::AddNode(const uint32_t parentId_val, const VgMath::Transform& transform_val)
{
	index++;

	Node node(index, transform_val, &nodes[parentId_val]);
	nodes[index] = node;

	return index;
}

void SceneGraph::DeleteNode(const uint32_t id_val)
{
	Node nodeToRemove = nodes[id_val];

	for (auto& node : nodes) {
		if (node.second.parent->id == id_val) {
			node.second.parent = nodeToRemove.parent;
		}
	}

	nodes.erase(id_val);
}

void SceneGraph::CalculateWorldTransforms()
{
	for (auto& node : nodes) {
		node.second.CalculateCumulativeTransform();
		node.second.modelMatrix = glm::mat4(1.0f);

		node.second.modelMatrix = glm::translate(
			node.second.modelMatrix, 
			glm::vec3(node.second.cumulativeTransform.translate.x, 
				node.second.cumulativeTransform.translate.y,
				node.second.cumulativeTransform.translate.z));

		node.second.modelMatrix = glm::rotate(
			node.second.modelMatrix,
			(float)node.second.cumulativeTransform.rotate.getAngleRadians(),
			glm::vec3(
				(float)node.second.cumulativeTransform.rotate.getAxis().x,
				(float)node.second.cumulativeTransform.rotate.getAxis().y,
				(float)node.second.cumulativeTransform.rotate.getAxis().z
			)
		);

		node.second.modelMatrix = glm::scale(
			node.second.modelMatrix,
#if ANISOTROPIC_SCALING
			glm::vec3(
				(float)node.second.cumulativeTransform.scale.x,
				(float)node.second.cumulativeTransform.scale.y,
				(float)node.second.cumulativeTransform.scale.z
			)
#else
			glm::vec3(
				(float)node.second.cumulativeTransform.scale
			)
#endif
		);

	}
}

