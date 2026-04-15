#include "DROP/terrain/terrainSystem.h"

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>

// std lib includes
#include <iostream>
#include <fstream>
#include <debugapi.h>

// Engine includes
#include "DROP/ECS/beecs.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/math/mat3.h"
#include "DROP/utils/ExecPath.h"


void TerrainSystem::InitTerrains(bseecs::ECS& ecs) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	size_t testSizeOf = sizeof(terrainContext.terrainToDisplacementMappings[0]);
	// Reset buffers
	memset(&terrainContext.terrainToDisplacementMappings[0], TERRAIN_INDEX_NULL
		, sizeof(terrainContext.terrainToDisplacementMappings[0]) * MAX_NUM_TERRAINS);

	memset(&terrainContext.requiredMaps, TERRAIN_INDEX_NULL
		, sizeof(terrainContext.requiredMaps[0]) * LOADED_MAPS);
	memset(&terrainContext.loadedMaps[0], TERRAIN_INDEX_NULL
		, sizeof(terrainContext.loadedMaps[0]) * LOADED_MAPS);
	memset(&terrainContext.stubTerrainDisplacementMap.displacementMap[0], 0.0
		, sizeof(terrainContext.stubTerrainDisplacementMap.displacementMap[0]) 
		* terrainContext.stubTerrainDisplacementMap.displacementMapSize);

	for (uint32_t i = 0; i < terrainContext.numOfLoadedTerrainDisplacementMaps; i++) {
		memset(&terrainContext.terrainDisplacementMaps[i].displacementMap[0], 0
			, sizeof(terrainContext.terrainDisplacementMaps[i].displacementMap[0]) 
			* terrainContext.terrainDisplacementMaps[i].displacementMapSize);
	}


	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();
	// Reset assets buffers
	memset(&terrainsAssetsContext.requiredMaps[0], TERRAIN_INDEX_NULL
		, sizeof(terrainsAssetsContext.requiredMaps[0]) * LOADED_MAPS);
	memset(&terrainsAssetsContext.loadedMaps[0], TERRAIN_INDEX_NULL
		, sizeof(terrainsAssetsContext.loadedMaps[0]) * LOADED_MAPS);

	for (uint32_t i = 0; i < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps; i++) {
		memset(&terrainsAssetsContext.terrainDisplacementMaps[i].displacementMap[0], 0
			, sizeof(terrainsAssetsContext.terrainDisplacementMaps[i].displacementMap[0])
			* terrainsAssetsContext.terrainDisplacementMaps[i].displacementMapSize);
	}


	TerrainSystem::InitTerrainsDisplacementMaps(terrainContext);
}

void TerrainSystem::UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();

	// Copy the assets terrain buffer into the game terrain buffer
	if (terrainContext.bNeedsDisplacementMapsUpdate) {

		// LOCK MUTEX
		assert(terrainContext.numOfLoadedTerrainDisplacementMaps == terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps);
		// Dest, Src, Size
		memcpy(
			&terrainContext.loadedMaps[0]
			, &terrainsAssetsContext.loadedMaps[0]
			, sizeof(terrainContext.loadedMaps[0]) * terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
		);
		memcpy(
			&terrainContext.terrainDisplacementMaps[0]
			, &terrainsAssetsContext.terrainDisplacementMaps[0]
			, sizeof(terrainContext.terrainDisplacementMaps[0]) * terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
		);
		// UNLOCK MUTEX

		//// this should be set false when all the displacement maps are loaded
		//bool bFound = true; // to true cause of FOR condition
		//for (uint32_t i = 0; 
		//	i < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
		//		&& bFound;
		//	i++
		//) {
		//	bFound = false;
		//	for (uint32_t j = 0;
		//		j < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
		//			&& !bFound;
		//		j++
		//	) {

		//	}
		//}
		//terrainContext.bNeedsDisplacementMapsUpdate = bFound;
	}

	// Calculate next maps needed

	// Find the next maps that should be needed

	// From the LOADED MAPS, find those that could be removed

	// Update the required maps
	TerrainID mapPosition = 0;
	TerrainID terrainPosition = 0;
	terrainsAssetsContext.loadedMaps[mapPosition] = TERRAIN_INDEX_NULL;
	TerrainDisplacementMap& currentTerrainDisplacementMap = terrainsAssetsContext.terrainDisplacementMaps[mapPosition];
	currentTerrainDisplacementMap.terrainIndex = terrainPosition;
	static int numLoads = 0;
	if (numLoads++ > 1)
		return;
	TerrainSystem::LoadTerrainDisplacementMap(
		&terrainsAssetsContext.terrainDisplacementMaps[mapPosition].displacementMap[0]
		, currentTerrainDisplacementMap.displacementMapSize
		, &terrainsAssetsContext.loadedMaps[mapPosition]
		, terrainPosition
	);

	// launch threads with the specific maps and a copy of the context
	
}

void TerrainSystem::InitTerrainsDisplacementMaps(TerrainsContext& inTerrainContext) {
	// Initialize terrains displacement maps
	for (uint32_t i = 0; i < inTerrainContext.numOfLoadedTerrainDisplacementMaps; i++) {
		for (uint32_t j = 0; j < inTerrainContext.terrainDisplacementMaps[i].displacementMapSize; j++) {
			inTerrainContext.terrainDisplacementMaps[i].displacementMap[j] =
				VgMath::RandomBetween0and1()
				* inTerrainContext.terrainDisplacementMaps[i].maxDisplacement;
		}
		// save in a file
	}

	// save in a file
	// Resolve file path
	std::string filePath = GetFullPath("/terrains/SomeTerrain.drop").c_str();
	float fileContent[TERRAIN_MAP_SIZE];

	// #TODO use the engine side specific function for file reading
	FILE* f;
	f = fopen(filePath.c_str(), "w");
	if (!f) {
		fclose(f);
		DebugBreak();
		return;
	}

	size_t sizeTest = sizeof(inTerrainContext.terrainDisplacementMaps[0].displacementMap[0]);
	size_t writtenBytes = fwrite(
		&inTerrainContext.terrainDisplacementMaps[0].displacementMap[0]
		, sizeof(inTerrainContext.terrainDisplacementMaps[0].displacementMap[0])
		, inTerrainContext.terrainDisplacementMaps[0].displacementMapSize
		, f
	);

	if (writtenBytes == 0) {
		DebugBreak();
	}
	
	fclose(f);
}

bool TerrainSystem::IsTerrainMapLoaded(
	TerrainsContext& terrainContext
	, TerrainID terrainID
) {
	bool outLoaded = false;

	if (terrainContext.terrainToDisplacementMappings[terrainID] != TERRAIN_INDEX_NULL) {
		// once is in one of the required maps,
		// its result is either zerod or the new displacement map
		outLoaded = true;
	}

	return outLoaded;
}

void TerrainSystem::LoadTerrainDisplacementMap(
	float* mapBuffer
	, uint32_t mapBufferSize
	, TerrainID* loadedMapPosToFill
	, TerrainID terrainPosition
) {
	if (terrainPosition == TERRAIN_INDEX_NULL) {
		DebugBreak();
		return;
	}

	if (!loadedMapPosToFill) {
		DebugBreak();
		return;
	}

	// Resolve file path
	std::string filePath = GetFullPath("/terrains/SomeTerrain.drop").c_str();
	float fileContent[TERRAIN_MAP_SIZE];

	// #TODO use the engine side specific function for file reading
	FILE* f;
	f = fopen(filePath.c_str(), "rb");
	if (!f) {
		DebugBreak();
		return;
	}

	fseek(f, 0, SEEK_END);
	uint32_t len = (uint32_t)ftell(f); // in bytes
	if (len < (sizeof(fileContent[0]) * mapBufferSize)) {
		fclose(f);
		DebugBreak();
		return;
	}

	rewind(f); // we moved to the end with fseek SEEK_END
	size_t bytesRead = fread(
		(void*)fileContent
		, sizeof(fileContent[0])
		, mapBufferSize
		, f
	);
	if (ferror(f)) {
		char* error;
		perror(error);
		clearerr(f);
		DebugBreak();
	}
	else if (feof(f)) {     /* possibility 2 */
		DebugBreak();
	}

	if (bytesRead == 0) {
		DebugBreak();
	}
	fclose(f);

	//ifstream fStream;
	//fStream.open(filePath);
	//std::stringstream stringtream;
	//stringtream << fStream.rdbuf();
	//fStream.close();

	// #TODO LOCK
	memcpy(
		mapBuffer
		, fileContent
		, sizeof(fileContent[0]) * mapBufferSize
	);
	// #TODO UnLOCK

	*loadedMapPosToFill = terrainPosition;
}