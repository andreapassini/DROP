#include "sceneGraph.h"

#include "vector3.h"
#include "versor3.h"
#include "mat3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>
#include <future> 

SceneGraph::SceneGraph(const uint32_t sizeEstimation)
{
	m_GameObjects.reserve(sizeEstimation);

	m_Index = 0;
	Node rootNode(m_Index, VgMath::Transform());	// Root or world always at index 0 

	m_GameObjects[m_Index] = rootNode;

	m_World = &m_GameObjects[m_Index];
}

uint32_t SceneGraph::AddObject(const uint32_t parentId_val)
{
	AddObject(parentId_val, VgMath::Transform());

	return m_Index;
}

uint32_t SceneGraph::AddObject(const uint32_t parentId_val, const VgMath::Transform& transform_val)
{
	m_Index++;

	Node* n = &m_GameObjects[parentId_val];

	Node node(m_Index, transform_val, n);
	m_GameObjects.insert({ m_Index , node });
	m_GameObjects[m_Index] = node;

	// To be removed
	Node nodeRef = m_GameObjects[m_Index];

	return m_Index;
}

// TODO: add an error return value or an option type
void SceneGraph::DeleteNode(const uint32_t id_val)
{
	Node nodeToRemove = m_GameObjects[id_val];

	// You are trying to remove the root node - "that's fucking illegal"
	if (nodeToRemove.m_Parent == nullptr) {	
		return;	
	}

	for (auto& node : m_GameObjects) {
		if (node.second.m_Parent != nullptr) {
			if (node.second.m_Parent->m_Id == id_val) {
				node.second.m_Parent = nodeToRemove.m_Parent;
			}
		}
	}

	int numberOfErasedNodes = m_GameObjects.erase(id_val);
}

// Calculate the cumulated transform starting from a local transform
// it also calculated the model matrix.
// It was not working with const ref
void SceneGraph::CalculateSingleWorldTransform(
	const Node& const node,
	VgMath::Transform* cumulatedTransform, 
	glm::mat4* modelMatrix)
{
	(*modelMatrix) = glm::mat4(1.0f);
	(*cumulatedTransform) = node.CalculateCumulativeTransform();

	(*modelMatrix) = glm::translate(
		(*modelMatrix),
		glm::vec3(
			(*cumulatedTransform).m_Translate.x,
			(*cumulatedTransform).m_Translate.y,
			(*cumulatedTransform).m_Translate.z
		)
	);

	(*modelMatrix) = glm::rotate(
		(*modelMatrix),
		(float)(*cumulatedTransform).m_Rotate.getAngleRadians(),
		glm::vec3(
			(float)(*cumulatedTransform).m_Rotate.getAxis().x,
			(float)(*cumulatedTransform).m_Rotate.getAxis().y,
			(float)(*cumulatedTransform).m_Rotate.getAxis().z
		)
	);

	(*modelMatrix) = glm::scale(
		(*modelMatrix),
#if ANISOTROPIC_SCALING
		glm::vec3(
			(float)(*cumulatedTransform).scale.x,
			(float)(*cumulatedTransform).scale.y,
			(float)(*cumulatedTransform).scale.z
		)
#else
		glm::vec3(
			(float)(*cumulatedTransform).m_Scale,
			(float)(*cumulatedTransform).m_Scale,
			(float)(*cumulatedTransform).m_Scale
		)
#endif
	);
}

void SceneGraph::CalculateWorldTransforms(
	std::unordered_map<uint32_t, VgMath::Transform>& const cumulatedTransforms,
	std::unordered_map<uint32_t, glm::mat4>& const modelMatrices) 
{
	std::vector<std::future<void>> futures;

	for (auto& it : m_GameObjects) {
		futures.push_back(
			std::async(std::launch::async, 
				CalculateSingleWorldTransform,
					it.second, 
					&cumulatedTransforms[it.first], 
					&modelMatrices[it.first]
			)
		);		
	}

	for (auto& handle : futures) {
		handle.wait();
	}
}

