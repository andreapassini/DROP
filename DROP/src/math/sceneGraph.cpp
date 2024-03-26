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

uint32_t SceneGraph::AddObject(const uint32_t parentId_val)
{
	AddObject(parentId_val, VgMath::Transform());

	return index;
}

uint32_t SceneGraph::AddObject(const uint32_t parentId_val, const VgMath::Transform& transform_val)
{
	index++;

	Node node(index, transform_val, &nodes[parentId_val]);
	nodes[index] = node;

	return index;
}

// TODO: add an error return value or an option type
void SceneGraph::DeleteNode(const uint32_t id_val)
{
	Node nodeToRemove = nodes[id_val];

	// You are trying to remove the root node - "that's fucking illegal"
	if (nodeToRemove.parent == nullptr) {	
		return;	
	}

	for (auto& node : nodes) {
		if (node.second.parent != nullptr) {
			if (node.second.parent->id == id_val) {
				node.second.parent = nodeToRemove.parent;
			}
		}
	}

	int numberOfErasedNodes = nodes.erase(id_val);
}

void SceneGraph::CalculateWorldTransforms()
{

	for (uint32_t i = 0; i <= index; i++) {
		
		nodes[i].CalculateCumulativeTransform();
		nodes[i].modelMatrix = glm::mat4(1.0f);

		nodes[i].modelMatrix = glm::translate(
			nodes[i].modelMatrix, 
			glm::vec3(
				nodes[i].cumulativeTransform.translate.x, 
				nodes[i].cumulativeTransform.translate.y,
				nodes[i].cumulativeTransform.translate.z
			)
		);

		//// To be removed
		//glm::vec3 t(
		//	nodes[i].cumulativeTransform.translate.x,
		//	nodes[i].cumulativeTransform.translate.y,
		//	nodes[i].cumulativeTransform.translate.z
		//);
		//std::cout << "t: " << t.x << ", " << t.y << ", " << t.z << std::endl;
		//std::cout << "M: " << "\n"
		//	<< nodes[i].modelMatrix[0].x << ", " << nodes[i].modelMatrix[0].y << ", " << nodes[i].modelMatrix[0].z << "\n"
		//	<< nodes[i].modelMatrix[1].x << ", " << nodes[i].modelMatrix[1].y << ", " << nodes[i].modelMatrix[1].z << "\n"
		//	<< nodes[i].modelMatrix[2].x << ", " << nodes[i].modelMatrix[2].y << ", " << nodes[i].modelMatrix[2].z << std::endl;

		nodes[i].modelMatrix = glm::rotate(
			nodes[i].modelMatrix,
			(float)nodes[i].cumulativeTransform.rotate.getAngleRadians(),
			glm::vec3(
				(float)nodes[i].cumulativeTransform.rotate.getAxis().x,
				(float)nodes[i].cumulativeTransform.rotate.getAxis().y,
				(float)nodes[i].cumulativeTransform.rotate.getAxis().z
			)
		);

		//// To be removed
		//glm::vec3 r(
		//	(float)nodes[i].cumulativeTransform.rotate.getAxis().x,
		//	(float)nodes[i].cumulativeTransform.rotate.getAxis().y,
		//	(float)nodes[i].cumulativeTransform.rotate.getAxis().z
		//);
		//std::cout << "r: " << r.x << ", " << r.y << ", " << r.z << "angle: " << (float)nodes[i].cumulativeTransform.rotate.getAngleRadians() << std::endl;
		//std::cout << "M: " << "\n"
		//	<< nodes[i].modelMatrix[0].x << ", " << nodes[i].modelMatrix[0].y << ", " << nodes[i].modelMatrix[0].z << "\n"
		//	<< nodes[i].modelMatrix[1].x << ", " << nodes[i].modelMatrix[1].y << ", " << nodes[i].modelMatrix[1].z << "\n"
		//	<< nodes[i].modelMatrix[2].x << ", " << nodes[i].modelMatrix[2].y << ", " << nodes[i].modelMatrix[2].z << std::endl;

		nodes[i].modelMatrix = glm::scale(
			nodes[i].modelMatrix,
#if ANISOTROPIC_SCALING
			glm::vec3(
				(float)nodes[i].cumulativeTransform.scale.x,
				(float)nodes[i].cumulativeTransform.scale.y,
				(float)nodes[i].cumulativeTransform.scale.z
			)
#else
			glm::vec3(
				(float)nodes[i].cumulativeTransform.scale
			)
#endif
		);

		//// To be removed
		//std::cout << "s: " << (float)nodes[i].cumulativeTransform.scale << std::endl;
		//std::cout << "M: " << "\n"
		//	<< nodes[i].modelMatrix[0].x << ", " << nodes[i].modelMatrix[0].y << ", " << nodes[i].modelMatrix[0].z << "\n"
		//	<< nodes[i].modelMatrix[1].x << ", " << nodes[i].modelMatrix[1].y << ", " << nodes[i].modelMatrix[1].z << "\n"
		//	<< nodes[i].modelMatrix[2].x << ", " << nodes[i].modelMatrix[2].y << ", " << nodes[i].modelMatrix[2].z << "\n" << std::endl;

	}
}

