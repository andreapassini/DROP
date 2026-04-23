#pragma once

#include "DROP/ECS/beecs.h"

typedef uint32_t TerrainID;
struct TerrainsContext;
struct TerrainsAssetsContext;

namespace VgMath {
	class Vector3;
}

namespace TerrainSystem {

void InitTerrains(bseecs::ECS& ecs);
void DiplaceTerrainComponent(bseecs::ECS& ecs);
void UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
);

void CalculateNearTargetIndecies(
	const VgMath::Vector3* const inTargetPosition
	, const float terrainDimension
	, const TerrainID numberOfTerrains
	, const TerrainID numberOfNearTargetIndecies
	, TerrainID* const OutIndeciesBuffer
);

void GenerateSaveAndSetPathForTerrainsDisplacementMaps(
	TerrainsContext& inTerrainContext
);

void InitTerrainsDisplacementMaps(TerrainsContext& inTerrainContext);

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

TerrainID PositionToIndex(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
);
TerrainID PositionToIndexClamped(
	const float terrainDimension
	, const TerrainID maxNumTerrains
	, const VgMath::Vector3* const inPosition
);

}