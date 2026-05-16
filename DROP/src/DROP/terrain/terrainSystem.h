#pragma once

#include "DROP/ECS/beecs.h"
#include "terrainComponent.h"

typedef uint32_t TerrainID;
struct TerrainsContext;
struct TerrainsAssetsContext;

namespace VgMath {
	class Vector3;
	class Transform;
}

namespace TerrainSystem {

void InitTerrains(bseecs::ECS& ecs);
void DiplaceTerrainComponent(bseecs::ECS& ecs);
void InitTargetPosition(bseecs::ECS& ecs);
void UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
);

void CalculateNearTargetIndexes(
	const TerrainGridIndex* const inTargetIndex
	, const float terrainDimension
	, const TerrainID numberOfTerrains
	, const TerrainID numberOfNearTargetIndexes
	, TerrainID* const OutIndexesBuffer
);

void GenerateSaveAndSetPathForTerrainsDisplacementMaps(
	TerrainsContext& inTerrainContext
);

void InitTerrainsDisplacementMaps(
	TerrainsContext& inTerrainContext
	, TerrainsAssetsContext& inTerrainsAssetsContext
);

bool IsTerrainMapLoaded(
	TerrainsContext& terrainContext
	, TerrainID terrainID
);

void LoadTerrainDisplacementMap(
	float* mapBuffer
	, uint32_t mapBufferSize
	, TerrainID* loadedMapPosToFill
	, TerrainID terrainPosition
	//, std::string filePath
);

void AsyncLoadTerrainDisplacementMap(
	float* mapBuffer
	, uint32_t mapBufferSize
	, TerrainID* loadedMapPosToFill
	, TerrainID terrainPosition
	, bool* bNewData
	//, std::string filePath
);

TerrainGridIndex PositionToIndex(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
);
TerrainGridIndex PositionToIndexClamped(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
);

}