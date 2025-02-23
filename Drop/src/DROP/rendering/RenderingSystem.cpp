#include "RenderingSystem.h"

#include "../rendering/model.h"
#include "../rendering/renderer.h"

using namespace Drop;

void RenderingSystem::Update(ECS& ecs, const float deltaTime) {
		
	std::vector<Model>& denseModels = ecs.GetComponentPool<Model>().Data();
	SceneContext& sceneContext = ecs.GetSingletonComponent<SceneContext>();

	
}
