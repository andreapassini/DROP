#include "sceneGraph.h"

#include "vector3.h"
#include "versor3.h"
#include "mat3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>

SceneGraph::SceneGraph(const uint32_t sizeEstimation)
{
	gameObjects.reserve(sizeEstimation);

	index = 0;
	Node rootNode(index, VgMath::Transform());	// Root or world always at index 0 

	gameObjects[index] = rootNode;
	world = &gameObjects[index];
}

uint32_t SceneGraph::AddObject(const uint32_t parentId_val)
{
	AddObject(parentId_val, VgMath::Transform());

	return index;
}

uint32_t SceneGraph::AddObject(const uint32_t parentId_val, const VgMath::Transform& transform_val)
{
	index++;

	Node* n = &gameObjects[parentId_val];

	Node node(index, transform_val, n);
	gameObjects[index] = node;

	// To be removed
	Node nodeRef = gameObjects[index];

	return index;
}

// TODO: add an error return value or an option type
void SceneGraph::DeleteNode(const uint32_t id_val)
{
	Node nodeToRemove = gameObjects[id_val];

	// You are trying to remove the root node - "that's fucking illegal"
	if (nodeToRemove.parent == nullptr) {	
		return;	
	}

	for (auto& node : gameObjects) {
		if (node.second.parent != nullptr) {
			if (node.second.parent->id == id_val) {
				node.second.parent = nodeToRemove.parent;
			}
		}
	}

	int numberOfErasedNodes = gameObjects.erase(id_val);
}

void SceneGraph::CalculateWorldTransforms(
	std::unordered_map<uint32_t, VgMath::Transform>& const cumulatedTransforms,
	std::unordered_map<uint32_t, glm::mat4>& const modelMatrices
) {

	for (auto& it : gameObjects) {
		modelMatrices[it.first] = glm::mat4(1.0f);
		cumulatedTransforms[it.first] = it.second.CalculateCumulativeTransform();

		modelMatrices[it.first] = glm::translate(
			modelMatrices[it.first],
			glm::vec3(
				cumulatedTransforms[it.first].translate.x,
				cumulatedTransforms[it.first].translate.y,
				cumulatedTransforms[it.first].translate.z
			)
		);

		modelMatrices[it.first] = glm::rotate(
			modelMatrices[it.first],
			(float)cumulatedTransforms[it.first].rotate.getAngleRadians(),
			glm::vec3(
				(float)cumulatedTransforms[it.first].rotate.getAxis().x,
				(float)cumulatedTransforms[it.first].rotate.getAxis().y,
				(float)cumulatedTransforms[it.first].rotate.getAxis().z
			)
		);

		modelMatrices[it.first] = glm::scale(
			modelMatrices[it.first],
#if ANISOTROPIC_SCALING
			glm::vec3(
				(float)cumulatedTransforms[it.first].scale.x,
				(float)cumulatedTransforms[it.first].scale.y,
				(float)cumulatedTransforms[it.first].scale.z
			)
#else
			glm::vec3(
				(float)cumulatedTransforms[it.first].scale,
				(float)cumulatedTransforms[it.first].scale,
				(float)cumulatedTransforms[it.first].scale
			)
#endif
		);

	}
}

