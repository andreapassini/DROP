#include "SceneSerializer.h"

// Dependencies includes
#include <yaml-cpp/yaml.h>

// std includes
#include <fstream>

// Engine includes
#include "Drop/sceneGraph/sceneGraph.h"
#include "Drop/rendering/staticMeshComponent.h"
#include "Drop/particles/particle.h"
#include "Drop/sceneGraph/scene.h"

namespace Drop
{

namespace SceneSerializer
{
	//YAML::Emitter& operator<<(YAML::Emitter& out, std::string& s)
	//{
	//	out << s.c_str();
	//	return out;
	//}
	YAML::Emitter& operator<<(YAML::Emitter& out, VgMath::Vector2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, VgMath::Vector3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, VgMath::Vector4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, VgMath::Quaternion& q)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << q.im << q.re << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, VgMath::Transform& transform)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Translate" << YAML::Value << transform.m_Translate;
		out << YAML::Key << "Rotate" << YAML::Value << transform.m_Rotate;
		out << YAML::Key << "Scale" << YAML::Value << transform.m_Scale;
		out << YAML::EndMap;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, Surface& s)
	{
		out << YAML::BeginMap;
		// The transform will be set at emit time
		out << YAML::Key << "m_Size" << s.m_Size;
		out << YAML::EndMap;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, ParticleStartValues& s)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "spawningSurface" << s.spawningSurface;
		out << YAML::Key << "lifeTime" << s.lifeTime;
		out << YAML::Key << "startsize" << s.startsize;
		out << YAML::Key << "endsize" << s.endsize;
		out << YAML::Key << "startSpeed" << s.startSpeed;
		out << YAML::Key << "endSpeed" << s.endSpeed;
		out << YAML::Key << "startColor" << s.startColor;
		out << YAML::Key << "endColor" << s.endColor;
		out << YAML::Key << "textureID" << s.textureID;
		out << YAML::EndMap;
		return out;
	}

	static void SerializeEntityAsText(
		YAML::Emitter& out
		, bseecs::ECS& ecs
		, const bseecs::EntityID entityId
	)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entityId; // the unique id here

		// For each components: serialize
#pragma region SerializeComponents
		if (ecs.Has<TransformComponent>(entityId))
		{
			out << YAML::Key << "TransformComponent";

			out << YAML::BeginMap;
			TransformComponent& currentTransformComp = ecs.Get<TransformComponent>(entityId);
			out << YAML::Key << "LocalTransform" << YAML::Value << currentTransformComp.m_CumulatedTransform;
			out << YAML::Key << "Parent" << YAML::Value << currentTransformComp.m_Parent;
			out << YAML::EndMap;

		}
		if (ecs.Has<StaticMeshComponent>(entityId))
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap;
			StaticMeshComponent& s = ecs.Get<StaticMeshComponent>(entityId);
			out << YAML::Key << "modelId" << YAML::Value << s.modelId;
			out << YAML::Key << "materialId" << YAML::Value << s.materialId;
			out << YAML::Key << "bCastShadow" << YAML::Value << s.bCastShadow;
			out << YAML::EndMap;
		}
		if (ecs.Has<ParticleEmitter>(entityId))
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap;
			ParticleEmitter& p = ecs.Get<ParticleEmitter>(entityId);
			out << YAML::Key << "numberOfParticles" << YAML::Value << p.numberOfParticles;
			out << YAML::Key << "particleToEmitEachTime" << YAML::Value << p.particleToEmitEachTime;
			out << YAML::Key << "lastIndex" << YAML::Value << p.lastIndex;
			out << YAML::Key << "spawningValues" << YAML::Value << p.spawningValues;
			out << YAML::EndMap;
		}
#pragma endregion

		out << YAML::EndMap; // -- Entity
	}

	void SceneSerializer::SerializeSceneAsText(
		const std::string& filePath
		, Scene* const scene
	)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << scene->sceneName.c_str();
		out << YAML::Key << "Entities" << YAML::BeginSeq;
		// For each registered entity in that scene
		for (EntityID i = 0; i < scene->ecs.GetMaxEntity(); i++)
		{
			const std::vector<EntityID>& availableEntities = scene->ecs.GetAvailableEntities();
			bool found = false;
			// Yah, is N search, kinda slow
			for (int32_t j = 0; i < availableEntities.size(); i++)
			{
				EntityID id = availableEntities[j];
				if (id == i)
				{
					found = true;
				}
			}
			if (!found)
			{
				SerializeEntityAsText(
					out
					, scene->ecs
					, i
				);
			}
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	bool SceneSerializer::DeserializeSceneFromText(
		const std::string& filePath
		, Scene* const scene
	)
	{
		return false;
	}

}

}