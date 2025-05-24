#pragma once

#include <string>


struct Scene;

// forward decl of namespace first
#include "Drop/ECS/beecs.h"
namespace YAML { class Emitter; }

namespace SceneSerializer
{
	void SerializeSceneAsText(const std::string& filePath, Scene* const scene);
	bool DeserializeSceneFromText(const std::string& filePath, Scene* const scene);


	void SerializeEntityAsText(
		YAML::Emitter& out
		, bseecs::ECS& ecs
		, const bseecs::EntityID entityId
	);
}