#pragma once

// std lib includes
#include <cstdint>

// Engine includes
#include "DROP/terrain/terrainComponent.h"


// This is used as the streaming target for terrains
// requires a Transform Component
struct TerrainTargetComponent
{
	// The id inside the terrain grid
	TerrainID linearizedID = TERRAIN_INDEX_NULL;
};