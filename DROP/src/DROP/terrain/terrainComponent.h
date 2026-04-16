#pragma once

#include <cstdint>


#define MAX_NUM_TERRAINS 121

#define TERRAIN_INDEX_NULL UINT32_MAX
#define LOADED_MAPS 9
#define TERRAIN_MAP_SIZE 81
#define TERRAIN_MAX_PATH_SIZE 256

typedef uint32_t ModelID;
typedef uint32_t MaterialID;
typedef uint32_t TerrainID;

struct TerrainDisplacementMap {
	//TerrainID terrainIndex = TERRAIN_INDEX_NULL;
	uint32_t displacementMapSize = TERRAIN_MAP_SIZE;
	float maxDisplacement = 2.5f;
	// #TODO use a texture and a 2DSampler
	float displacementMap[TERRAIN_MAP_SIZE]; // this depends on num of vertices of the mesh
};

struct TerrainDisplacementPath {
	char filePath[TERRAIN_MAX_PATH_SIZE];
	size_t maxFilePathSize = TERRAIN_MAX_PATH_SIZE;
};

// Use a mutex to access TerrainDisplacementMaps
struct TerrainsAssetsContext {
	TerrainID numOfLoadedTerrainDisplacementMaps = LOADED_MAPS;
	// useful for fast lookup
	TerrainID requiredMaps[LOADED_MAPS];
	TerrainID loadedMaps[LOADED_MAPS];
	TerrainDisplacementMap terrainDisplacementMaps[LOADED_MAPS];
};

struct TerrainsContext {
	TerrainID numOfLoadedTerrainDisplacementMaps = LOADED_MAPS;
	bool bNeedsDisplacementMapsUpdate = false;

	// like a sparse set
	TerrainID terrainToDisplacementMappings[MAX_NUM_TERRAINS];
	TerrainID maxNumTerrains = MAX_NUM_TERRAINS;

	// useful for fast lookup
	TerrainID requiredMaps[LOADED_MAPS];
	TerrainID loadedMaps[LOADED_MAPS];

	TerrainDisplacementMap terrainDisplacementMaps[LOADED_MAPS];

	// stub displacement map
	TerrainDisplacementMap stubTerrainDisplacementMap;
	TerrainDisplacementPath terrainDisplacementPath[MAX_NUM_TERRAINS];
};


struct TerrainComponent {
	ModelID modelId = 0;

	// #TODO 
	// Maybe extend to an array of materials
	// so each mesh of the model can have its material
	// or simply add the Material inside each mesh of the model
	MaterialID materialId = 0;

	// how much data regarding rendering specifics
	// has to be stored in this component?
	bool bCastShadow = false;

	TerrainID numOfTerrains = MAX_NUM_TERRAINS;
};
