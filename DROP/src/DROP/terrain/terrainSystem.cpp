#include "DROP/terrain/terrainSystem.h"

#include "DROP/ECS/beecs.h"
#include "DROP/terrain/terrainComponent.h"
#include "DROP/math/mat3.h"


void InitTerrains(bseecs::ECS& ecs) {
	// Initialize terrains displacement maps
	std::vector<TerrainComponent>& denseTerrainComponents = ecs.GetComponentPool<TerrainComponent>().Data();

	for (TerrainComponent& currentTerrain : denseTerrainComponents) {
		for (size_t i = 0; i < currentTerrain.numOfTerrains; i++) {
			for (size_t j = 0; j < currentTerrain.displacementMaps[i].displacementMapSize; j++) {
				currentTerrain.displacementMaps[i].displacementMap[j] = 
					VgMath::RandomBetween0and1() 
					* currentTerrain.displacementMaps[i].maxDisplacement;
			}
		}
	}
}