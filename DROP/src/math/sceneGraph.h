#pragma once

#include "transform.h"

#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Node {
public:
	uint32_t id;
	VgMath::Transform localTransform;
	Node* parent;

	VgMath::Transform cumulativeTransform;
	glm::mat4 modelMatrix;

	Node() : id(), parent(nullptr), modelMatrix(glm::mat4(1.0f)) {

	}

	Node(const uint32_t id_val) :
		id(id_val), parent(nullptr), 
		localTransform(), cumulativeTransform(), modelMatrix(glm::mat4(1.0f))
		{ };

	Node(const uint32_t id_val, const VgMath::Transform transform_val) :
		id(id_val), parent(nullptr), 
		localTransform(transform_val), cumulativeTransform(), modelMatrix(glm::mat4(1.0f))
		{ };

	Node(const uint32_t id_val, const VgMath::Transform transform_val, Node* parent_val) :
		id(id_val), parent(parent_val), 
		localTransform(transform_val), cumulativeTransform(), modelMatrix(glm::mat4(1.0f))
		{ };

	~Node() {};

	void operator=(const Node& n) {
		this->id = n.id;
		this->parent = n.parent;
		this->localTransform = n.localTransform;
		//Node(n.id, n.localTransform, n.parent);
	}

	void CalculateCumulativeTransform() {
		modelMatrix = glm::mat4(1.0f);

		modelMatrix = glm::translate(
			modelMatrix,
			glm::vec3(
				localTransform.translate.x,
				localTransform.translate.y,
				localTransform.translate.z
			)
		);

		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(localTransform.rotate[3]),
			glm::vec3(
				localTransform.rotate.x,
				localTransform.rotate.y,
				localTransform.rotate.z
			)
		);

		modelMatrix = glm::scale(
			modelMatrix,
#if ANISOTROPIC_SCALING
			glm::vec3(
				(float)cumulativeTransform.scale.x,
				(float)cumulativeTransform.scale.y,
				(float)cumulativeTransform.scale.z
			)
#else
			glm::vec3(
				localTransform.scale,
				localTransform.scale,
				localTransform.scale
			)
#endif
		);

		if (this->parent != nullptr) {
			modelMatrix = this->parent->GetTransformWorldCoordinates() * modelMatrix;
		}
		else {
			modelMatrix = modelMatrix;
		}
	}

	glm::mat4 GetTransformWorldCoordinates() {
		CalculateCumulativeTransform();
		return modelMatrix;
	};

private:

};

class SceneGraph {
private:
	Node* world;
	uint32_t index;

public:
	// Wanna keep this so i can easily find the one i need and loop on through every node
	std::unordered_map<uint32_t, Node> gameObjects;

	SceneGraph(const uint32_t sizeEstimation);
	~SceneGraph() {};

	uint32_t AddObject(const uint32_t parentId);
	uint32_t AddObject(const uint32_t parentId, const VgMath::Transform& transform);

	void DeleteNode(const uint32_t id_val);

	void CalculateWorldTransforms();

	static constexpr uint32_t ROOT_ID = 0;
};

