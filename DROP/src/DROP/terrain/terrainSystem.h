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
void UpdateTerrains(
	bseecs::ECS& ecs
	, const float deltaTime
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

TerrainID PositionInIndex(
	const TerrainsContext* const inTerrainContext
	, const VgMath::Vector3* const inPosition
);

}