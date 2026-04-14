#pragma once

#include "DROP/ECS/beecs.h"

typedef uint32_t TerrainID;
struct TerrainsContext;

namespace TerrainSystem {

void InitTerrains(bseecs::ECS& ecs);
void UpdateTerrains(bseecs::ECS& ecs);

void InitTerrainsDisplacementMaps(TerrainsContext& inTerrainContext);

bool IsTerrainMapLoaded(
	bseecs::ECS& ecs
	, TerrainID terrainID
);

}