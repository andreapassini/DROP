#include "sceneGraph.h"
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/string_cast.hpp>

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

void SceneGraph::CalculateWorldTransforms()
{

	for (uint32_t i = 0; i <= index; i++) {
		////// To be removed
		//std::cout << "Key: " << i << "\n"
		//	<< glm::to_string(gameObjects[i].modelMatrix) << std::endl;

		gameObjects[i].CalculateCumulativeTransform();
//		gameObjects[i].modelMatrix = glm::mat4(1.0f);
//
//		gameObjects[i].modelMatrix = glm::translate(
//			gameObjects[i].modelMatrix, 
//			glm::vec3(
//				gameObjects[i].cumulativeTransform.translate.x, 
//				gameObjects[i].cumulativeTransform.translate.y,
//				gameObjects[i].cumulativeTransform.translate.z
//			)
//		);
//
//		gameObjects[i].modelMatrix = glm::rotate(
//			gameObjects[i].modelMatrix,
//			glm::radians((float)gameObjects[i].cumulativeTransform.rotate.getAngleDegree()),
//			glm::vec3(
//				(float)gameObjects[i].cumulativeTransform.rotate.getAxis().x,
//				(float)gameObjects[i].cumulativeTransform.rotate.getAxis().y,
//				(float)gameObjects[i].cumulativeTransform.rotate.getAxis().z
//			)
//		);
//
//		gameObjects[i].modelMatrix = glm::scale(
//			gameObjects[i].modelMatrix,
//#if ANISOTROPIC_SCALING
//			glm::vec3(
//				(float)gameObjects[i].cumulativeTransform.scale.x,
//				(float)gameObjects[i].cumulativeTransform.scale.y,
//				(float)gameObjects[i].cumulativeTransform.scale.z
//			)
//#else
//			glm::vec3(
//				(float)gameObjects[i].cumulativeTransform.scale
//			)
//#endif
//		);
	}
}

