#pragma once

#include "transform.h"

#include <vector>
#include <collection.h>

#include "glm/glm.hpp"

class Node {
public:
	const uint32_t id;
	VgMath::Transform localTransform;
	Node* parent;

	Node(const uint32_t id_val) :
		id(id_val), parent(nullptr), 
		localTransform(), cumulativeTransform(), modelMatrix(glm::mat4(1.0f))
		{ };

	Node(const uint32_t id_val, const VgMath::Transform transform_val) :
		id(id_val), parent(nullptr), 
		localTransform(transform_val), cumulativeTransform(), modelMatrix(glm::mat4(1.0f))
		{ };

	Node(const uint32_t id_val, const VgMath::Transform transform_val, Node* const parent_val) :
		id(id_val), parent(parent_val), 
		localTransform(transform_val), cumulativeTransform(), modelMatrix(glm::mat4(1.0f))
		{ };

	~Node() {};

	void operator=(const Node& n) {
		Node(n.id, n.localTransform);
	}

	void CalculateCumulativeTransform() {
		if (this->parent != nullptr) {
			cumulativeTransform = cumulativeTransform * this->parent->GetTransformWorldCoordinates();
		}
	}

	VgMath::Transform GetTransformWorldCoordinates() {
		CalculateCumulativeTransform();
		return cumulativeTransform;
	};

	VgMath::Transform cumulativeTransform;
	glm::mat4 modelMatrix;

private:

};

class SceneGraph {
private:
	Node* world;
	uint32_t index;

	// Wanna keep this so i can easily find the one i need and loop on through every node
	std::unordered_map<uint32_t, Node> nodes;

public:
	SceneGraph(const uint32_t sizeEstimation);
	~SceneGraph() {};

	uint32_t AddNode(const uint32_t parentId);
	uint32_t AddNode(const uint32_t parentId, const VgMath::Transform& transform);

	void DeleteNode(const uint32_t id_val);

	void CalculateWorldTransforms();
};

