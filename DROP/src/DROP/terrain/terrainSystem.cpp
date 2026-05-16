#include "DROP/terrain/terrainSystem.h"

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h> // #TODO do not include win here, use the platform independent layer call
#include <debugapi.h>

// std lib includes
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>

// Engine includes
#include "DROP/core/thread.h"
#include "DROP/ECS/beecs.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/terrain/terrainTargetComponent.h"
#include "DROP/math/mat3.h"
#include "DROP/utils/ExecPath.h"
#include "DROP/core/file.h"
#include "DROP/math/vector3.h"
#include "DROP/sceneGraph/sceneGraph.h"
#include "DROP/assetManager/assetManager.h"


// #TODO move this into the terrain context
std::mutex gMutex;

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

	memset(&terrainContext.stubTerrainDisplacementMap.displacementMap[0], 0.0f
		, sizeof(terrainContext.stubTerrainDisplacementMap.displacementMap[0]) 
			* LOADED_MAPS);

	for (uint32_t i = 0; i < terrainContext.numOfLoadedTerrainDisplacementMaps; i++) {
		memset(&terrainContext.terrainDisplacementMaps[i].displacementMap[0], 0.0f
			, sizeof(terrainContext.terrainDisplacementMaps[i].displacementMap[0]) 
				* LOADED_MAPS);
	}

	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();
	// Reset assets buffers
	memset(
		&terrainsAssetsContext.requiredMaps[0], TERRAIN_INDEX_NULL
		, sizeof(terrainsAssetsContext.requiredMaps[0]) * LOADED_MAPS);

	memset(&terrainsAssetsContext.loadedMaps[0], TERRAIN_INDEX_NULL
		, sizeof(terrainsAssetsContext.loadedMaps[0]) * LOADED_MAPS);

	for (uint32_t i = 0; i < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps; i++) {
		memset(&terrainsAssetsContext.terrainDisplacementMaps[i].displacementMap[0], 0.0f
			, sizeof(terrainsAssetsContext.terrainDisplacementMaps[i].displacementMap[0])
			* LOADED_MAPS);
	}

	TerrainSystem::InitTerrainsDisplacementMaps(
		terrainContext
		, terrainsAssetsContext
	);
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
	float gridCenter = (float)numRowOrCol * 0.5f;
	float displacement = (terrainEdgeSize * gridCenter) - (terrainEdgeSize * 0.5f);
	 
	// Goal: Move the grid to place grid center in world (0.0, 0.0, 0.0)
	// Grid starting from bot left
	// 0.0  o - - > X
	//      |
	//		|
	//		v Z
	// Terrain transform (-displacement, 0.0, -displacement)
	VgMath::Vector3 extraDisplacement = VgMath::Vector3(-displacement, 0.0f, 0.0f) + VgMath::Vector3(0.0f, 0.0f, displacement);
	transformComp.localTransform.translate = transformComp.localTransform.translate + extraDisplacement; // minus since we are rendering them from bot left
}

void TerrainSystem::InitTargetPosition(
	bseecs::ECS& ecs
) {
	TerrainsContext& terrainsContext = ecs.GetSingletonComponent<TerrainsContext>();
	VgMath::Transform& currentTargetTransform = ecs.Get<TransformComponent>(terrainsContext.targetID).localTransform;

	// Move the specified debug target to position
// #TODO assuming a single TerrainTarget
	std::vector<TerrainComponent>& denseTerrainComponents = ecs.GetComponentPool<TerrainComponent>().Data();
	EntityID DenseID = 0;
	TerrainComponent& terrainComponent = denseTerrainComponents[DenseID];
	TransformComponent& terrainTransformComp = ecs.GetSibiling<TerrainComponent, TransformComponent>(DenseID);
	//

	// Init Old position
	VgMath::Transform currentTransform = currentTargetTransform;
	currentTransform.translate = currentTransform.translate - terrainTransformComp.localTransform.translate;
	TerrainGridIndex currentTargetIndex = PositionToIndex(
		terrainsContext.terrainDimension
		, terrainsContext.maxNumTerrains
		, &currentTransform.translate
	);
	terrainsContext.oldTargetIndex = currentTargetIndex;

	VgMath::Transform& currentDebugPosTransform = ecs.Get<TransformComponent>(terrainsContext.debugPosID).localTransform;
	currentDebugPosTransform = terrainTransformComp.localTransform;
}

void BounceTarget(
	VgMath::Transform& currentTargetTransform
	, TransformComponent& terrainTransformComp
	, VgMath::Vector3& outTargetPosition
	, const float deltaTime
) {
	float xLeftLimit = -(121.0f / 2.0);
	float xRightLimit = 121.0f / 2.0;
	float speed = 7.5f;
	// Bouncing on X
	static bool bGoingRight = true;
	//if (currentTargetTransform.translate.x < xRightLimit && bGoingRight)
	//{
	//	currentTargetTransform.translate.x += (speed * deltaTime);
	//}
	//else if (currentTargetTransform.translate.x >= xLeftLimit && !bGoingRight)
	//{
	//	currentTargetTransform.translate.x -= (speed * deltaTime);
	//}
	//else
	//{
	//	bGoingRight = !bGoingRight;
	//}

	// Go at random
	static bool bFirstTime = true;
	static float timer = 3.0f;
	static float currentTime = 0.0f;
	currentTime += deltaTime;
	if (!bFirstTime || currentTime < timer)
	{
		return;
	}

	currentTargetTransform.translate.x = VgMath::randomBetween(xLeftLimit, xRightLimit);
	currentTime = 0.0f;
	bFirstTime = false;

	// Move it into terrainComp space/world
	VgMath::Transform targetTransformInCompSpace = terrainTransformComp.localTransform * currentTargetTransform;
	outTargetPosition = -targetTransformInCompSpace.translate;
}

void MoveTargetToRandomSpot(
	VgMath::Transform& currentTargetTransform
	, TransformComponent& terrainTransformComp
	, VgMath::Vector3& outTargetPosition
	, const float deltaTime
	, bool bMoveOnce
) {
	float xLeftLimit = -(121.0f / 4.0f);
	float xRightLimit = 121.0f / 4.0f;
	float speed = 7.5f;
	bool bMovingOnX = true;
	bool bMovingOnZ = true;

	// Go at random
	static bool bFirstTime = true;
	static float timer = 5.0f;
	static float currentTime = 0.0f;
	currentTime += deltaTime;

	if ((bMoveOnce && !bFirstTime) || currentTime < timer)
	{
		return;
	}
	
	if (bMovingOnX)
	{
		//currentTargetTransform.translate.x = trunc(VgMath::randomBetween(xLeftLimit, xRightLimit));
		currentTargetTransform.translate.x = -30.0f;
	}
	
	if (bMovingOnZ)
	{
		currentTargetTransform.translate.z = trunc(VgMath::randomBetween(xLeftLimit, xRightLimit));
		currentTargetTransform.translate.z = -30.0f;
	}

	currentTime = 0.0f;
	bFirstTime = false;

	// Move it into terrainComp space/world
	//VgMath::Transform targetTransformInCompSpace = terrainTransformComp.localTransform * currentTargetTransform;
	outTargetPosition = currentTargetTransform.translate;
}

// Loaders threads -> inTerrainsAssetsContext
// use inTerrainsAssetsContext as a double buffer not to lock every time the main thread operations
void UpdateTerrainContextWithLoadedMaps(
	TerrainsContext& inTerrainsContext
	, TerrainsAssetsContext& inTerrainsAssetsContext
) {
	TerrainID mappingToClear[LOADED_MAPS];

	{
		std::unique_lock<std::mutex> lock(gMutex);

		if (!inTerrainsAssetsContext.bNewData)
		{
			return;
		}

		// Copy for mapping
		memcpy(
			&mappingToClear[0]
			, &inTerrainsContext.loadedMaps[0]
			, sizeof(inTerrainsContext.loadedMaps[0]) * inTerrainsContext.numOfLoadedTerrainDisplacementMaps
		);

		// copy the loaded map buffers
		assert(inTerrainsContext.numOfLoadedTerrainDisplacementMaps == inTerrainsAssetsContext.numOfLoadedTerrainDisplacementMaps);
		memcpy(
			&inTerrainsContext.terrainDisplacementMaps[0]
			, &inTerrainsAssetsContext.terrainDisplacementMaps[0]
			, sizeof(inTerrainsContext.terrainDisplacementMaps[0]) * inTerrainsContext.numOfLoadedTerrainDisplacementMaps
		);

		// copy also the loaded maps
		memcpy(
			&inTerrainsContext.loadedMaps[0]
			, &inTerrainsAssetsContext.loadedMaps[0]
			, sizeof(inTerrainsContext.loadedMaps[0]) * inTerrainsContext.numOfLoadedTerrainDisplacementMaps
		);
	
		inTerrainsAssetsContext.bNewData = false;
	}

	// update mapping
	for (TerrainID i = 0; i < LOADED_MAPS; i++)
	{
		if (mappingToClear[i] != TERRAIN_INDEX_NULL)
		{
			inTerrainsContext.terrainToDisplacementMappings[mappingToClear[i]] = TERRAIN_INDEX_NULL;
		}
	}
	for (TerrainID i = 0; i < inTerrainsContext.numOfLoadedTerrainDisplacementMaps; i++)
	{
		if (inTerrainsContext.loadedMaps[i] != TERRAIN_INDEX_NULL)
		{
			inTerrainsContext.terrainToDisplacementMappings[inTerrainsContext.loadedMaps[i]] = i;
		}
	}
}

void TerrainSystem::UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
) {
	TerrainsContext& terrainsContext = ecs.GetSingletonComponent<TerrainsContext>();
	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();
	
	UpdateTerrainContextWithLoadedMaps(
		terrainsContext
		, terrainsAssetsContext
	);

	VgMath::Transform& currentTargetTransform = ecs.Get<TransformComponent>(terrainsContext.targetID).localTransform;

	// #TODO assuming a single TerrainTarget
	std::vector<TerrainComponent>& denseTerrainComponents = ecs.GetComponentPool<TerrainComponent>().Data();
	EntityID DenseID = 0;
	TerrainComponent& terrainComponent = denseTerrainComponents[DenseID];
	TransformComponent& terrainTransformComp = ecs.GetSibiling<TerrainComponent, TransformComponent>(DenseID);
	//

#if BOUNCE
	BounceTarget(
		currentTargetTransform
		, terrainTransformComp
		, targetPosition
		, deltaTime
	);
#else
	// Test move -24, -26
	MoveTargetToRandomSpot(
		currentTargetTransform
		, terrainTransformComp
		, currentTargetTransform.translate
		, deltaTime
		, true // Move once
	);
#endif
	// Check target position in grid
	// if it the same as the old one, skip
	VgMath::Transform currentTransform;
	currentTransform.translate = (-terrainTransformComp.localTransform.translate + currentTargetTransform.translate);
	currentTransform.translate.z = -currentTransform.translate.z; // since Z towards cam is positive

	TerrainGridIndex currentTargetIndex = PositionToIndex(
		terrainsContext.terrainDimension
		, terrainsContext.maxNumTerrains
		, &currentTransform.translate
	);
	TerrainID currentTargetLinearizedIndex = currentTargetIndex.linearizedIndex;
	if (terrainsContext.oldTargetIndex.linearizedIndex == TERRAIN_INDEX_NULL
		|| terrainsContext.oldTargetIndex.linearizedIndex == currentTargetLinearizedIndex)
	{
		return;
	}
	else
	{
		terrainsContext.oldTargetIndex = currentTargetIndex;
	}

	CalculateNearTargetIndexes(
		&currentTargetIndex
		, terrainsContext.terrainDimension
		, terrainsContext.maxNumTerrains
		, LOADED_MAPS // must be bufferSize
		, terrainsContext.requiredMaps
	);

	// Do we need to update loaded maps?
	TerrainID numOfNewMapsRequired = LOADED_MAPS;
	for (TerrainID i = 0; i < LOADED_MAPS; i++) {
		for (TerrainID j = 0; j < LOADED_MAPS; j++) {
			if (terrainsContext.loadedMaps[j] == terrainsContext.requiredMaps[i]) {
				//terrainsContext.requiredMaps[i] = TERRAIN_INDEX_NULL;
				numOfNewMapsRequired--;
			}
			else if (TERRAIN_INDEX_NULL == terrainsContext.requiredMaps[i])
			{
				numOfNewMapsRequired--;
				break;
			}
		}
	}

	if (numOfNewMapsRequired <= 0 || numOfNewMapsRequired > LOADED_MAPS) {
		return;
	}

	{
		TerrainID gridEdge = (TerrainID)(sqrt(terrainsContext.maxNumTerrains));
		std::cout << "row: " << currentTargetLinearizedIndex / gridEdge << std::endl; 
		std::cout << "col: " << currentTargetLinearizedIndex % gridEdge << std::endl;

		std::cout << "Req: " << std::endl;
		std::cout << "[" << std::endl;
		for (TerrainID i = 0; i < LOADED_MAPS; i++)
		{
			std::cout << "\t" << terrainsContext.requiredMaps[i] << std::endl;
		}
		std::cout << "]" << std::endl;
	}

	// Find which new maps we need and sub them with the old ones
	TerrainID freeLoadedMapsIndexes[LOADED_MAPS];
	for (TerrainID i = 0; i < LOADED_MAPS; i++)
	{
		bool bFound = false;
		for (TerrainID j = 0; j < LOADED_MAPS && !bFound; j++)
		{
			if (TERRAIN_INDEX_NULL == terrainsContext.requiredMaps[j])
			{
				continue;
			}

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

	// Clear not needed maps
	for (TerrainID i = 0; i < LOADED_MAPS; i++)
	{
		for (TerrainID j = 0; j < LOADED_MAPS && TERRAIN_INDEX_NULL != terrainsContext.requiredMaps[i]; j++)
		{
			if (terrainsContext.loadedMaps[j] == terrainsContext.requiredMaps[i])
			{
				terrainsContext.requiredMaps[i] = TERRAIN_INDEX_NULL;
			}
		}
	}

	AssetManagerContext& assetManagerContext = ecs.GetSingletonComponent<AssetManagerContext>();
	
	TerrainID lastFreeID = 0;
	for (TerrainID i = 0; i < LOADED_MAPS; i++)
	{
		// Loop on Free Loaded Maps Indexes
		if (TERRAIN_INDEX_NULL == terrainsContext.requiredMaps[i]) continue;

		bool bLoaded = false;
		for (TerrainID j = lastFreeID; j < LOADED_MAPS && bLoaded == false; j++)
		{
			if (TERRAIN_INDEX_NULL == freeLoadedMapsIndexes[j]) continue;

			lastFreeID = j + 1;

			// use TerrainsAssetContext for multi-threaded solution
			TerrainID mapToBeLoaded = terrainsContext.requiredMaps[i];
			TerrainID indexOfMapToBeLoaded = freeLoadedMapsIndexes[j];
			TerrainID* mapPositionInLoaded = &terrainsAssetsContext.loadedMaps[indexOfMapToBeLoaded];

			float* mapBuffer = &terrainsAssetsContext.terrainDisplacementMaps[indexOfMapToBeLoaded].displacementMap[0];
			TerrainID mapSize = terrainsContext.terrainDisplacementMaps[indexOfMapToBeLoaded].displacementMapSize;
			bool* bNewData = &terrainsAssetsContext.bNewData;

			AssetManager::Enqueue(
				&assetManagerContext
				, [
					mapBuffer
					, mapSize
					, mapPositionInLoaded
					, mapToBeLoaded
					, bNewData
				] {
					AsyncLoadTerrainDisplacementMap(
						mapBuffer
						, mapSize
						, mapPositionInLoaded
						, mapToBeLoaded
						, bNewData
					);
				}
			);
			
			bLoaded = true;
		}
	}

	//std::cout << "Terrain update" << std::endl;
}

// assuming OutIndexesBuffer.num == numberOfNearTargetIndexes
void TerrainSystem::CalculateNearTargetIndexes(
	const TerrainGridIndex* const inTargetIndex
	, const float terrainDimension
	, const TerrainID numberOfTerrains
	, const TerrainID numberOfNearTargetIndexes
	, TerrainID* const OutIndexesBuffer
) {
	assert(inTargetIndex);

	int32_t gridEdge = (int32_t)(sqrt(numberOfTerrains));
	int32_t targetRow = inTargetIndex->row;
	int32_t targetCol = inTargetIndex->col;

	// we are subtracting, use int
	TerrainID iteration = 0;
	int32_t dimSizeIncrease = (int32_t)(sqrt(LOADED_MAPS))/2;
	for (int32_t row = targetRow - dimSizeIncrease; row <= targetRow + dimSizeIncrease; row++) {

		if (row < 0 || row >= gridEdge) {
			continue;
		}

		for (int32_t col = targetCol -dimSizeIncrease; col <= targetCol + dimSizeIncrease; col++) {

			if (col < 0 || col >= gridEdge) {
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
	, TerrainsAssetsContext& inTerrainsAssetsContext
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
	TerrainID dimSizeIncrease = (TerrainID)(sqrt(LOADED_MAPS)) / 2;
	for (TerrainID row = centerRow - dimSizeIncrease; row <= centerRow + dimSizeIncrease; row++) {
		for (TerrainID col = centerCol - dimSizeIncrease; col <= centerCol + dimSizeIncrease; col++) {
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

	// Copy the loaded in TerrainAssetContext
	// Not using mutex since it's the start
	memcpy(
		&inTerrainsAssetsContext.loadedMaps[0]
		, &inTerrainContext.loadedMaps[0]
		, LOADED_MAPS * sizeof(inTerrainContext.loadedMaps[0])
	);
	memcpy(
		&inTerrainsAssetsContext.requiredMaps[0]
		, &inTerrainContext.requiredMaps[0]
		, LOADED_MAPS * sizeof(inTerrainContext.requiredMaps[0])
	);
	memcpy(
		&inTerrainsAssetsContext.terrainDisplacementMaps[0]
		, &inTerrainContext.terrainDisplacementMaps[0]
		, LOADED_MAPS * sizeof(inTerrainContext.terrainDisplacementMaps[0])
	);
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

	//std::cout << "reading file: " << filePath << std::endl;

	size_t bufferElementSize = sizeof(fileContent[0]);
	File::ReadBinaryFile(
		filePath
		, (char*)(&fileContent[0])
		, bufferElementSize, mapBufferSize
	);

	memcpy(
		mapBuffer
		, fileContent
		, sizeof(float) * mapBufferSize
	);
	*loadedMapPosToFill = terrainPosition;
}

void TerrainSystem::AsyncLoadTerrainDisplacementMap(
	float* mapBuffer
	, uint32_t mapBufferSize
	, TerrainID* loadedMapPosToFill
	, TerrainID terrainPosition
	, bool* bNewData
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

	//std::cout << "reading file: " << filePath << std::endl;

	size_t bufferElementSize = sizeof(fileContent[0]);
	File::ReadBinaryFile(
		filePath
		, (char*)(&fileContent[0])
		, bufferElementSize, mapBufferSize
	);

	{
		std::unique_lock<std::mutex> lock(gMutex);
		memcpy(
			mapBuffer
			, fileContent
			, sizeof(float) * mapBufferSize
		);
		*loadedMapPosToFill = terrainPosition;
		(*bNewData) = true;
	}

	//sleep_ms(150);
}

// InPos must be in TerrainCompSpace
TerrainGridIndex TerrainSystem::PositionToIndex(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
) {
	TerrainGridIndex outTerrainID;
	if (!inPosition)
	{
		DebugBreak();
		return outTerrainID;
	}

	// Grid calculation
	TerrainID edgeSize = TERRAIN_EDGE_SIZE;
	TerrainID col = (TerrainID)(abs(inPosition->x) / edgeSize); // ABS should not be needed since we should move everything in terrain comp space
	TerrainID row = (TerrainID)(abs(inPosition->z) / edgeSize);
	TerrainID numCol = (TerrainID)(sqrt(maxNumTerrains)); // sqrt should handle neg numbers

	outTerrainID.linearizedIndex = col + (row * numCol);
	outTerrainID.row = row;
	outTerrainID.col = col;

	return outTerrainID;
}

TerrainGridIndex TerrainSystem::PositionToIndexClamped(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
) {
	TerrainGridIndex outTerrainID;
	TerrainID numCol = (TerrainID)(sqrt(maxNumTerrains)); // sqrt should handle neg numbers

	outTerrainID = PositionToIndex(
		terrainDimension
		, maxNumTerrains
		, inPosition
	);

	outTerrainID.linearizedIndex = clamp<TerrainID>(outTerrainID.linearizedIndex, 0, maxNumTerrains);
	outTerrainID.row = clamp<TerrainID>(outTerrainID.linearizedIndex, 0, numCol);
	outTerrainID.col = clamp<TerrainID>(outTerrainID.linearizedIndex, 0, numCol);

	return outTerrainID;
}