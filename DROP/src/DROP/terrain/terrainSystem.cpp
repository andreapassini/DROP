#include "DROP/terrain/terrainSystem.h"

#include "DROP/ECS/beecs.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/math/mat3.h"


void TerrainSystem::InitTerrains(bseecs::ECS& ecs) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	size_t testSizeOf = sizeof(terrainContext.terrainToDisplacementMappings[0]);
	// Reset buffers
	memset(&terrainContext.terrainToDisplacementMappings[0], 0
		, sizeof(terrainContext.terrainToDisplacementMappings[0]) * MAX_NUM_TERRAINS);

	memset(&terrainContext.requiredMaps, 0
		, sizeof(terrainContext.requiredMaps[0]) * LOADED_MAPS);

	assert(0);
	// FIX THIS - we are also resetting all the data
	memset(&terrainContext.terrainDisplacementMaps[0], 0
		, sizeof(terrainContext.terrainDisplacementMaps[0]) * LOADED_MAPS);

	memset(&terrainContext.terrainDisplacementMaps[0], 0
		, sizeof(terrainContext.terrainDisplacementMaps[0]) * LOADED_MAPS);


	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();
	// Reset assets buffers
	memset(&terrainsAssetsContext.requiredMaps[0], 0
		, sizeof(terrainsAssetsContext.requiredMaps[0]) * LOADED_MAPS);

	memset(&terrainsAssetsContext.terrainDisplacementMaps[0], 0
		, sizeof(terrainsAssetsContext.terrainDisplacementMaps[0]) * LOADED_MAPS);


	TerrainSystem::InitTerrainsDisplacementMaps(terrainContext);
}

void TerrainSystem::UpdateTerrains(bseecs::ECS& ecs) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();

	// Copy the assets terrain buffer into the game terrain buffer
	if (terrainContext.bNeedsDisplacementMapsUpdate) {
		// LOCK MUTEX
		assert(terrainContext.numOfLoadedTerrainDisplacementMaps == terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps);
		// Dest, Src, Size
		memcpy(
			&terrainContext.terrainDisplacementMaps[0]
			, &terrainsAssetsContext.terrainDisplacementMaps[0]
			, sizeof(terrainContext.terrainDisplacementMaps[0]) * terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
		);
		// UNLOCK MUTEX
	}

	// Calculate next maps needed
	terrainContext.bNeedsDisplacementMapsUpdate = false;
}

void TerrainSystem::InitTerrainsDisplacementMaps(TerrainsContext& inTerrainContext) {
	// Initialize terrains displacement maps
	for (uint32_t i = 0; i < inTerrainContext.numOfLoadedTerrainDisplacementMaps; i++) {
		for (uint32_t j = 0; j < inTerrainContext.terrainDisplacementMaps[i].displacementMapSize; j++) {
			inTerrainContext.terrainDisplacementMaps[i].displacementMap[j] =
				VgMath::RandomBetween0and1()
				* inTerrainContext.terrainDisplacementMaps[i].maxDisplacement;
		}
	}
}

bool TerrainSystem::IsTerrainMapLoaded(
	bseecs::ECS& ecs
	, TerrainID terrainID
) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	bool outLoaded = false;
	assert(0);
	//if (terrainContext.loadSatuses[terrainID] == LoadStatus::LOADED) {
	//	outLoaded = true;
	//}

	return outLoaded;
}
