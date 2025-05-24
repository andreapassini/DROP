#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

#include "Drop/sceneGraph/sceneGraph.h"
#include "Drop/rendering/staticMeshComponent.h"

// To Be Removed
struct Scene
{
	bseecs::ECS ecs;
};
//

void SceneSerializer::SerializeEntityAsText(
	YAML::Emitter& out
	, bseecs::ECS& ecs
	, const bseecs::EntityID entityId
) {
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity" << YAML::Value << "12837192831273"; // the unique id here

	// For each components: serialize
#pragma region SerializeComponents
	if (ecs.Has<TransformComponent>(entityId))
	{

	}	
	if (ecs.Has<StaticMeshComponent>(entityId))
	{

	}
#pragma endregion

	out << YAML::EndMap; // -- Entity
}

void SceneSerializer::SerializeSceneAsText(
	const std::string& filePath
	, Scene* const scene
) {
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Name";
	out << YAML::Key << "Entities" << YAML::BeginSeq;
	// For each registered entity in that scene
	SerializeEntityAsText(
		out
		, scene->ecs
		, bseecs::NULL_ENTITY
	);
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filePath);
	fout << out.c_str();
}

bool SceneSerializer::DeserializeSceneFromText(
	const std::string& filePath
	, Scene* const scene
) {
	return false;
}
