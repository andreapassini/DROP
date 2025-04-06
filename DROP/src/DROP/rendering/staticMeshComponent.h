#pragma once
#include <cstdint>

// attach this comp to entity
// maybe rename it to Static Model
// since it is not holding a mesh directly

typedef uint32_t ModelID;
typedef uint32_t MaterialID;

struct MeshComponent {
	ModelID modelId = 0;

	// #TODO 
	// Maybe extend to an array of materials
	// so each mesh of the model can have its material
	MaterialID materialId = 0;

	// how much data regarding rendering specifics
	// has to be stored in this component?
	bool bCastShadow = false;
};
