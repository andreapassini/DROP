#pragma once

#include "transform.h"
#include "vector3.h"
#include "versor3.h"
#include "mat3.h"

#include <vector>
#include <unordered_map>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Node {
public:
	uint32_t id;
	VgMath::Transform localTransform;
	Node* parent;

	Node() : id(), parent(nullptr), 
		localTransform() {

	}

	Node(const uint32_t id_val) :
		id(id_val), parent(nullptr), 
		localTransform()
		{ };

	Node(const uint32_t id_val, const VgMath::Transform transform_val) :
		id(id_val), parent(nullptr), 
		localTransform(transform_val)
		{ };

	Node(const uint32_t id_val, const VgMath::Transform transform_val, Node* parent_val) :
		id(id_val), parent(parent_val), 
		localTransform(transform_val)
		{ };

	~Node() {};

	void operator=(const Node& n) {
		this->id = n.id;
		this->parent = n.parent;
		this->localTransform = n.localTransform;
		//Node(n.id, n.localTransform, n.parent);
	}

	VgMath::Transform CalculateCumulativeTransform() const {

		VgMath::Transform cumulativeTransform;
		cumulativeTransform = localTransform;

		if (this->parent != nullptr) {	// Has a parent
			if(this->parent->id != 0){	// The parent is not the world
				cumulativeTransform = this->parent->CalculateCumulativeTransform() * localTransform;
			}
		}

		return cumulativeTransform;
	}

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

	void CalculateWorldTransforms(
		std::unordered_map<uint32_t, VgMath::Transform>& const cumulatedTransforms,
		std::unordered_map<uint32_t, glm::mat4>& const modelMatrices
	);

	static constexpr uint32_t ROOT_ID = 0;

private:
	static void CalculateSingleWorldTransform(
		const Node& const node,
		VgMath::Transform* cumulatedTransform,
		glm::mat4* modelMatrix);
};

