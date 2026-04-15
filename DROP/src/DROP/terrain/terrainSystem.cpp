#include "DROP/terrain/terrainSystem.h"

#include "DROP/ECS/beecs.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/math/mat3.h"


void TerrainSystem::InitTerrains(bseecs::ECS& ecs) {
	TerrainsContext& terrainContext = ecs.GetSingletonComponent<TerrainsContext>();
	size_t testSizeOf = sizeof(terrainContext.terrainToDisplacementMappings[0]);
	// Reset buffers
	memset(&terrainContext.terrainToDisplacementMappings[0], TERRAIN_INDEX_NULL
		, sizeof(terrainContext.terrainToDisplacementMappings[0]) * MAX_NUM_TERRAINS);

	memset(&terrainContext.requiredMaps, TERRAIN_INDEX_NULL
		, sizeof(terrainContext.requiredMaps[0]) * LOADED_MAPS);

	for (uint32_t i = 0; i < terrainContext.numOfLoadedTerrainDisplacementMaps; i++) {
		memset(&terrainContext.terrainDisplacementMaps[i].displacementMap[0], 0
			, sizeof(terrainContext.terrainDisplacementMaps[i].displacementMap[0]) 
			* terrainContext.terrainDisplacementMaps[i].displacementMapSize);
	}


	TerrainsAssetsContext& terrainsAssetsContext = ecs.GetSingletonComponent<TerrainsAssetsContext>();
	// Reset assets buffers
	memset(&terrainsAssetsContext.requiredMaps[0], TERRAIN_INDEX_NULL
		, sizeof(terrainsAssetsContext.requiredMaps[0]) * LOADED_MAPS);

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
			&terrainContext.terrainDisplacementMaps[0]
			, &terrainsAssetsContext.terrainDisplacementMaps[0]
			, sizeof(terrainContext.terrainDisplacementMaps[0]) * terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
		);
		assert(0);
		// COPY ALSO THE LOADED MAPS
		// UNLOCK MUTEX


		// this should be set false when all the displacement maps are loaded
		bool bFound = true; // to true cause of FOR condition
		for (uint32_t i = 0; 
			i < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
				&& bFound;
			i++
		) {
			bFound = false;
			for (uint32_t j = 0;
				j < terrainsAssetsContext.numOfLoadedTerrainDisplacementMaps
					&& !bFound;
				j++
			) {

			}
		}
		terrainContext.bNeedsDisplacementMapsUpdate = bFound;
	}

	// Calculate next maps needed
	
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
