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
#include "DROP/core/file.h"


void TerrainSystem::InitTerrains(bseecs::ECS& ecs) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();

	size_t sizeOfTerrainsContext = sizeof(terrainContext);

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
		memset(&terrainContext.terrainDisplacementMaps[i].displacementMap[0], 0.0
			, sizeof(terrainContext.terrainDisplacementMaps[i].displacementMap[0]) 
				* terrainContext.terrainDisplacementMaps[i].displacementMapSize);
	}

	//for (uint32_t i = 0; i < terrainContext.maxNumTerrains; i++) {
	//	memset(&terrainContext.terrainDisplacementPath[i].filePath[0], 0
	//		, sizeof(terrainContext.terrainDisplacementPath[i].filePath[0])
	//		* terrainContext.terrainDisplacementPath[i].maxFilePathSize);
	//}


	//TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();
	//// Reset assets buffers
	//memset(&terrainsAssetsContext.requiredMaps[0], TERRAIN_INDEX_NULL
	//	, sizeof(terrainsAssetsContext.requiredMaps[0]) * LOADED_MAPS);

	//memset(&terrainsAssetsContext.loadedMaps[0], TERRAIN_INDEX_NULL
	//	, sizeof(terrainsAssetsContext.loadedMaps[0]) * LOADED_MAPS);

	//for (uint32_t i = 0; i < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps; i++) {
	//	memset(&terrainsAssetsContext.terrainDisplacementMaps[i].displacementMap[0], 0.0
	//		, sizeof(terrainsAssetsContext.terrainDisplacementMaps[i].displacementMap[0])
	//		* terrainsAssetsContext.terrainDisplacementMaps[i].displacementMapSize);
	//}


	TerrainSystem::InitTerrainsDisplacementMaps(terrainContext);
}

void TerrainSystem::UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
) {
	return;

	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();

	// Copy the assets terrain buffer into the game terrain buffer
	if (terrainContext.bNeedsDisplacementMapsUpdate && false) {

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
	//TerrainID mapPosition = 0;
	//TerrainID terrainPosition = 0;
	//terrainsAssetsContext.loadedMaps[mapPosition] = TERRAIN_INDEX_NULL;
	//TerrainDisplacementMap& currentTerrainDisplacementMap = terrainsAssetsContext.terrainDisplacementMaps[mapPosition];
	//currentTerrainDisplacementMap.terrainIndex = terrainPosition;
	//static int numLoads = 0;
	//if (numLoads++ > 1)
	//	return;
	//TerrainSystem::LoadTerrainDisplacementMap(
	//	&terrainsAssetsContext.terrainDisplacementMaps[mapPosition].displacementMap[0]
	//	, currentTerrainDisplacementMap.displacementMapSize
	//	, &terrainsAssetsContext.loadedMaps[mapPosition]
	//	, terrainPosition
	//	, terrainContext
	//);

	// launch threads with the specific maps and a copy of the context
	
}

// #TODO Divide the path creation from map generation
void TerrainSystem::GenerateSaveAndSetPathForTerrainsDisplacementMaps(
	TerrainsContext& inTerrainContext
) {
	//  #TODO make it parallel
	float displacementMap[TERRAIN_MAP_SIZE]; // this depends on num of vertices of the mesh
	for (uint32_t i = 0; i < inTerrainContext.maxNumTerrains; i++)
	{
		// Fill the temp buffer and save it
		for (uint32_t j = 0; j < inTerrainContext.terrainDisplacementMaps[0].displacementMapSize; j++)
		{
			displacementMap[j] =
				VgMath::RandomBetween0and1()
				* 2.5f;
			assert(displacementMap[j] >= 0.0 && displacementMap[j] <= 2.5);
		}
		std::string relPath = "/terrains/terrainDisplacement_"
			+ std::to_string(i)
			+ ".drop";

		std::string absProjPath = GetRelativeProjectPathWithMarker();
		std::string filePath = absProjPath + relPath;
		File::WriteFile(
			//filePath.c_str(), filePath.size() + 1 // for the null terminator
			filePath
			, (char*)&displacementMap[0]
			, sizeof(displacementMap[0])
			, inTerrainContext.terrainDisplacementMaps[0].displacementMapSize
		);

		inTerrainContext.terrainDisplacementPath[i] = std::move(filePath);

		memset(
			&displacementMap[0]
			, 0.0
			, sizeof(displacementMap[0]) * inTerrainContext.terrainDisplacementMaps[0].displacementMapSize
		);

		//assert(TERRAIN_MAX_PATH_SIZE >= filePath.size() + 1); // safety check
		//memcpy(
		//	&inTerrainContext.terrainDisplacementPath[i].filePath[0]
		//	, filePath.c_str()
		//	, filePath.size() + 1
		//);
		//inTerrainContext.terrainDisplacementPath[i].maxFilePathSize = filePath.size() + 1;
	}
}

// SYNC INIT
void TerrainSystem::InitTerrainsDisplacementMaps(
	TerrainsContext& inTerrainContext
) {

#if 1 // not only for maps re-generation but also for paths
	// #TODO Divide the path creation from map generation
	GenerateSaveAndSetPathForTerrainsDisplacementMaps(inTerrainContext);
#endif

	TerrainID numCol = sqrt(inTerrainContext.maxNumTerrains);
	TerrainID numRow = sqrt(inTerrainContext.maxNumTerrains);
	TerrainID centerRow = numRow / 2;
	TerrainID centerCol = numCol / 2;
	TerrainID centerId = numCol / 2 + (numRow / 2 * numRow);

	TerrainID currentIndex = 0;
	uint32_t iteration = 0;
	for (TerrainID row = centerRow - 1; row <= centerRow + 1; row++) {
		for (TerrainID col = centerCol - 1; col <= centerCol + 1; col++) {
			currentIndex = col + row * numCol;
			LoadTerrainDisplacementMap(
				(char*)&inTerrainContext.terrainDisplacementMaps[iteration].displacementMap[0]
				//, inTerrainContext.terrainDisplacementMaps[iteration].displacementMapSize
				, sizeof(float)
				, &inTerrainContext.loadedMaps[iteration]
				, currentIndex
				//, inTerrainContext.terrainDisplacementPath[currentIndex]
				//, &inTerrainContext.terrainDisplacementPath[currentIndex].filePath[0]
				//, inTerrainContext.terrainDisplacementPath[currentIndex].maxFilePathSize
			);
			inTerrainContext.terrainToDisplacementMappings[currentIndex] = iteration;
			iteration++;
		}
	}
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

// #TODO for file, use platform specific functions
void TerrainSystem::LoadTerrainDisplacementMap(
	char* mapBuffer
	, uint32_t mapBufferSize
	, TerrainID* loadedMapPosToFill
	, TerrainID terrainPosition
	//, std::string filePath
)
{
	if (TERRAIN_INDEX_NULL == terrainPosition)
	{
		DebugBreak();
		return;
	}

	if (!loadedMapPosToFill)
	{
		DebugBreak();
		return;
	}

	// Resolve file path
	//std::string relPath = "/terrains/terrainDisplacement_"
	//	+ std::to_string(terrainPosition)
	//	+ ".drop";
	//std::string absProjPath = GetRelativeProjectPathWithMarker();
	//std::string filePath = absProjPath + relPath;

	char fileContent[TERRAIN_MAP_SIZE];

	std::string relPath = "/terrains/terrainDisplacement_"
		+ std::to_string(terrainPosition)
		+ ".drop";
	std::string absProjPath = GetRelativeProjectPathWithMarker();
	std::string filePath = absProjPath + relPath;

	std::cout << "reading file: " << filePath << std::endl;

	// #TODO use the engine side specific function for file reading
	FILE* f;
	f = fopen(filePath.c_str(), "rb");
	if (!f)
	{
		DebugBreak();
		return;
	}

	fseek(f, 0, SEEK_END);
	uint32_t len = (uint32_t)ftell(f); // in bytes
	if (len < (sizeof(fileContent[0]) * mapBufferSize))
	{
		fclose(f);
		DebugBreak();
		return;
	}

	rewind(f); // we moved to the end with fseek SEEK_END
	size_t bytesRead = fread(
		&fileContent[0]
		, sizeof(fileContent[0])
		, mapBufferSize
		, f
	);
	if (ferror(f))
	{
		char* error;
		perror(error);
		clearerr(f);
		DebugBreak();
	}
	else if (feof(f))
	{     /* possibility 2 */
		DebugBreak();
	}

	if (bytesRead == 0)
	{
		DebugBreak();
	}

	//// Test with fscanf
	//rewind(f); // we moved to the end with fseek SEEK_END
	//for (size_t i = 0; i < mapBufferSize; i++)
	//{
	//	fscanf(f, "%lf", &fileContent[i]);
	//	printf("%.3f ", fileContent[i]);
	//}

	fclose(f);

	// #TODO LOCK
	memcpy(
		mapBuffer
		, fileContent
		, sizeof(float) * mapBufferSize
	);
	// #TODO UnLOCK

	*loadedMapPosToFill = terrainPosition;
}