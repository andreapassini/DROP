#include "sceneGraph.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

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

	Node* n = &nodes[parentId_val];

	Node node(index, transform_val, n);
	nodes[index] = node;

	// To be removed
	Node nodeRef = nodes[index];

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

		// To be removed
		std::cout << "\n" << "key: " << i << std::endl;
		glm::vec3 t(
			nodes[i].cumulativeTransform.translate.x,
			nodes[i].cumulativeTransform.translate.y,
			nodes[i].cumulativeTransform.translate.z
		);
		std::cout << "t: " << t.x << ", " << t.y << ", " << t.z << std::endl;
		std::cout << "M: " << "\n"
			<< glm::to_string(nodes[i].modelMatrix) << std::endl;

		nodes[i].modelMatrix = glm::rotate(
			nodes[i].modelMatrix,
			glm::radians((float)nodes[i].cumulativeTransform.rotate.getAngleDegree()),
			glm::vec3(
				(float)nodes[i].cumulativeTransform.rotate.getAxis().x,
				(float)nodes[i].cumulativeTransform.rotate.getAxis().y,
				(float)nodes[i].cumulativeTransform.rotate.getAxis().z
			)
		);

		// To be removed
		glm::vec3 r(
			(float)nodes[i].cumulativeTransform.rotate.getAxis().x,
			(float)nodes[i].cumulativeTransform.rotate.getAxis().y,
			(float)nodes[i].cumulativeTransform.rotate.getAxis().z
		);
		std::cout << "r: " << r.x << ", " << r.y << ", " << r.z << "\n" <<
			" cos (a/2): " << (float)nodes[i].cumulativeTransform.rotate.re << "\n" <<
			" angle rad: " << glm::radians((float)nodes[i].cumulativeTransform.rotate.getAngleDegree()) << "\n" <<
			" angle deg: " << (float)nodes[i].cumulativeTransform.rotate.getAngleDegree() << std::endl;
		std::cout << "M: " << "\n"
			<< glm::to_string(nodes[i].modelMatrix) << std::endl;

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
		std::cout << "M: " << "\n"
			<< glm::to_string(nodes[i].modelMatrix) << std::endl;
		int a = 5;
	}
}

