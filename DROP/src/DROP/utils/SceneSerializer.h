#pragma once

#include <string>


struct Scene;

// forward decl of namespace first
#include "DROP/ECS/beecs.h"

namespace Drop
{

namespace SceneSerializer
{
	void SerializeSceneAsText(
		const std::string& filePath
		, Scene* const scene
	);
	bool DeserializeSceneFromText(
		const std::string& filePath
		, Scene* const scene
	);
}

}