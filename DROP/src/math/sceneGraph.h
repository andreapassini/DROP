#pragma once

#include "transform.h"

#include <vector>
#include <collection.h>

class Node {
	uint32_t id;
	VgMath::Transform localTransform;
	VgMath::Transform cumulativeTransform;
	Node* parent;
};

class SceneGraph
{
	Node* root;

	// Wanna keep this so i can easily find the one i need and loop on through every node
	std::unordered_map<uint32_t, Node> nodes;

public:
	SceneGraph(uint32_t sizeEstimation);
	~SceneGraph();
	uint32_t AddNode();
};

