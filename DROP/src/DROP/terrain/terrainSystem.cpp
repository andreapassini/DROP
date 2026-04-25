#include "DROP/terrain/terrainSystem.h"

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h> // #TODO do not include win here, use the platform independent layer call
#include <debugapi.h>

// std lib includes
#include <iostream>
#include <fstream>

// Engine includes
#include "DROP/ECS/beecs.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/terrain/terrainTargetComponent.h"
#include "DROP/math/mat3.h"
#include "DROP/utils/ExecPath.h"
#include "DROP/core/file.h"
#include "DROP/math/vector3.h"
#include "DROP/sceneGraph/sceneGraph.h"

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

void TerrainSystem::DiplaceTerrainComponent(
	bseecs::ECS& ecs
) {
	// #TODO For now assume one terrain component
	// Move the terrain component based on grid size
	TerrainsContext& terrainsContext = ecs.GetSingletonComponent<TerrainsContext>();
	std::vector<TerrainComponent>& denseTerrainComponents = ecs.GetComponentPool<TerrainComponent>().Data();
	EntityID DenseID = 0;
	TerrainComponent& terrainComponent = denseTerrainComponents[DenseID];
	TransformComponent& transformComp = ecs.GetSibiling<TerrainComponent, TransformComponent>(DenseID);

	int32_t numRowOrCol = (int32_t)sqrt(terrainComponent.numOfTerrains); // since it's a square
	float terrainEdgeSize = TERRAIN_EDGE_SIZE;
	float gridCenter = (float)numRowOrCol / 2.0f;
	float displacement = terrainEdgeSize * gridCenter;
	 
	VgMath::Vector3 extraDisplacement = VgMath::Vector3(displacement, 0.0f, 0.0f) + VgMath::Vector3(0.0f, 0.0f, displacement);
	transformComp.localTransform.translate = transformComp.localTransform.translate - extraDisplacement; // minus since we are rendering them from bot left
}



void TerrainSystem::UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
) {
	TerrainsContext& terrainsContext = ecs.GetSingletonComponent<TerrainsContext>();

	float xLeftLimit = -(121.0f / 2.0);
	float xRightLimit = 121.0f / 2.0;
	float speed = 15.0f;
	VgMath::Transform& currentTargetTransform = ecs.Get<TransformComponent>(terrainsContext.targetID).localTransform;
	// Bouncing on X
	static bool bGoingRight = true;
	if (currentTargetTransform.translate.x < xRightLimit && bGoingRight)
	{
		currentTargetTransform.translate.x += (speed * deltaTime);
	}
	else if(currentTargetTransform.translate.x >= xLeftLimit && !bGoingRight)
	{
		currentTargetTransform.translate.x -= (speed * deltaTime);
	}
	else
	{
		bGoingRight = !bGoingRight;
	}

	// #TODO assuming a single TerrainTarget
	std::vector<TerrainComponent>& denseTerrainComponents = ecs.GetComponentPool<TerrainComponent>().Data();
	EntityID DenseID = 0;
	TerrainComponent& terrainComponent = denseTerrainComponents[DenseID];
	TransformComponent& terrainTransformComp = ecs.GetSibiling<TerrainComponent, TransformComponent>(DenseID);
	//

	// Move it into terrainComp space/world
	VgMath::Transform targetTransformInCompSpace = terrainTransformComp.localTransform * ecs.Get<TransformComponent>(terrainsContext.targetID).localTransform;
	VgMath::Vector3 targetPosition = -targetTransformInCompSpace.translate;
	CalculateNearTargetIndexes(
		&targetPosition
		, terrainsContext.terrainDimension
		, terrainsContext.maxNumTerrains
		, LOADED_MAPS // must be bufferSize
		, terrainsContext.requiredMaps
	);

	// Do we need to update loaded maps?
	TerrainID numOfNewMapsRequired = 0;
	for (TerrainID i = 0; i < LOADED_MAPS; i++) {
		numOfNewMapsRequired++;		
		for (TerrainID j = 0; j < LOADED_MAPS && TERRAIN_INDEX_NULL != terrainsContext.requiredMaps[i]; j++) {
			if (terrainsContext.loadedMaps[j] == terrainsContext.requiredMaps[i]) {
				terrainsContext.requiredMaps[i] = TERRAIN_INDEX_NULL;
				numOfNewMapsRequired--;
			}
		}
	}

	if (numOfNewMapsRequired == 0) {
		return;
	}

	// Find which new maps we need and sub them with the old ones
	TerrainID freeLoadedMapsIndexes[LOADED_MAPS];
	for (TerrainID i = 0; i < LOADED_MAPS; i++)
	{
		bool bFound = false;
		for (TerrainID j = 0; j < LOADED_MAPS && !bFound && TERRAIN_INDEX_NULL != terrainsContext.requiredMaps[j]; j++)
		{
			if (terrainsContext.loadedMaps[i] == terrainsContext.requiredMaps[j])
			{
				bFound = true;
			}
		}

		if (!bFound)
		{
			freeLoadedMapsIndexes[i] = i;
		}
		else
		{
			freeLoadedMapsIndexes[i] = TERRAIN_INDEX_NULL;
		}
	}
	
	for (TerrainID i = 0; i < LOADED_MAPS; i++)
	{
		// Loop on Free Loaded Maps Indexes
		if (TERRAIN_INDEX_NULL == freeLoadedMapsIndexes[i]) continue;
		if (TERRAIN_INDEX_NULL == terrainsContext.requiredMaps[i]) continue;

		TerrainID mapToBeLoaded = terrainsContext.requiredMaps[i];
		TerrainID indexOfMapToBeLoaded = freeLoadedMapsIndexes[i];
		TerrainID* mapPositionInLoaded = &terrainsContext.loadedMaps[indexOfMapToBeLoaded];

		// Clear old mapping
		terrainsContext.terrainToDisplacementMappings[terrainsContext.loadedMaps[indexOfMapToBeLoaded]] = TERRAIN_INDEX_NULL;


		// Load "mapToBeLoaded" in "mapPositionInLoaded" of "terrainsContext.loadedMaps"

		LoadTerrainDisplacementMap(
			&terrainsContext.terrainDisplacementMaps[indexOfMapToBeLoaded].displacementMap[0]
			, terrainsContext.terrainDisplacementMaps[indexOfMapToBeLoaded].displacementMapSize
			, mapPositionInLoaded
			, mapToBeLoaded
		);

		// Update mapping
		terrainsContext.terrainToDisplacementMappings[terrainsContext.loadedMaps[indexOfMapToBeLoaded]] = indexOfMapToBeLoaded;
	}
}

// assuming OutIndexesBuffer.num == numberOfNearTargetIndexes
void TerrainSystem::CalculateNearTargetIndexes(
	const VgMath::Vector3* const inTargetPosition
	, const float terrainDimension
	, const TerrainID numberOfTerrains
	, const TerrainID numberOfNearTargetIndexes
	, TerrainID* const OutIndexesBuffer
) {
	TerrainID targetLinearizedIndex = PositionToIndex(
		terrainDimension
		, numberOfTerrains
		, inTargetPosition
	);
	TerrainID gridEdge = (TerrainID)(sqrt(numberOfTerrains));
	TerrainID targetRow = targetLinearizedIndex / gridEdge;
	TerrainID targetCol = targetLinearizedIndex % gridEdge;

	// we are subtracting, use int
	TerrainID iteration = 0;
	for (int32_t row = (int32_t)(targetRow) - 1; row <= targetRow + 1; row++) {
		if (row < 0 || row > gridEdge) {
			continue;
		}

		for (int32_t col = (int32_t)(targetCol) - 1; col <= targetCol + 1; col++) {
			if (col < 0 || col > gridEdge) {
				continue;
			}

			int32_t currentIndex = col + row * (int32_t)(gridEdge);
			OutIndexesBuffer[iteration] = currentIndex;
			iteration++;
		}
	}
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
		File::WriteBinaryFile(
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
				&inTerrainContext.terrainDisplacementMaps[iteration].displacementMap[0]
				//, inTerrainContext.terrainDisplacementMaps[iteration].displacementMapSize
				, inTerrainContext.terrainDisplacementMaps[iteration].displacementMapSize
				, &inTerrainContext.loadedMaps[iteration]
				, currentIndex
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
	float* mapBuffer
	, uint32_t mapBufferSize
	, TerrainID* loadedMapPosToFill
	, TerrainID terrainPosition
	//, std::string filePath
) {
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

	// since this could go in another thread
	// we dont want to lock the map buffer while reading file
	float fileContent[TERRAIN_MAP_SIZE];

	std::string relPath = "/terrains/terrainDisplacement_"
		+ std::to_string(terrainPosition)
		+ ".drop";
	std::string absProjPath = GetRelativeProjectPathWithMarker();
	std::string filePath = absProjPath + relPath;

	std::cout << "reading file: " << filePath << std::endl;

	size_t bufferElementSize = sizeof(fileContent[0]);
	File::ReadBinaryFile(
		filePath
		, (char*)(&fileContent[0])
		, bufferElementSize, mapBufferSize
	);

	// #TODO LOCK
	memcpy(
		mapBuffer
		, fileContent
		, sizeof(float) * mapBufferSize
	);
	*loadedMapPosToFill = terrainPosition;
	// #TODO UnLOCK

}

TerrainID TerrainSystem::PositionToIndex(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
) {
	TerrainID outTerrainID = TERRAIN_INDEX_NULL;
	if (!inPosition)
	{
		DebugBreak();
		return outTerrainID;
	}

	// Grid calculation
	TerrainID gridEdge = (TerrainID)(sqrt(maxNumTerrains));
	TerrainID col = (TerrainID)(inPosition->x / gridEdge);
	TerrainID row = (TerrainID)(inPosition->z / gridEdge);
	TerrainID numCol = gridEdge;

	outTerrainID = col + (row * numCol);

	if (outTerrainID >= maxNumTerrains)
	{
		outTerrainID = TERRAIN_INDEX_NULL;
	}

	return outTerrainID;
}

TerrainID TerrainSystem::PositionToIndexClamped(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
) {
	TerrainID outTerrainID = TERRAIN_INDEX_NULL;

	outTerrainID = PositionToIndex(
		terrainDimension
		, maxNumTerrains
		, inPosition
	);

	outTerrainID = clamp<TerrainID>(outTerrainID, 0, maxNumTerrains);

	return outTerrainID;
}