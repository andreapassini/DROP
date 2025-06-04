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
#include "Drop/tags/tag.h"
#include "DROP/utils/Log.h"
#include "ExecPath.h"

using namespace VgMath;

namespace YAML
{
	template<>
	struct convert<VgMath::Vector2>
	{
		static Node encode(const VgMath::Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
		}
		static bool decode(const Node& node, VgMath::Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;
			
			rhs.x = node[0].as<VgMath::Scalar>();
			rhs.y = node[1].as<VgMath::Scalar>();
		}
	};
	template<>
	struct convert<VgMath::Vector3>
	{
		static Node encode(const VgMath::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
		}
		static bool decode(const Node& node, VgMath::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<VgMath::Scalar>();
			rhs.y = node[1].as<VgMath::Scalar>();
			rhs.z = node[2].as<VgMath::Scalar>();
		}
	};
	template<>
	struct convert<VgMath::Vector4>
	{
		static Node encode(const VgMath::Vector4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
		}
		static bool decode(const Node& node, VgMath::Vector4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<VgMath::Scalar>();
			rhs.y = node[1].as<VgMath::Scalar>();
			rhs.z = node[2].as<VgMath::Scalar>();
			rhs.w = node[3].as<VgMath::Scalar>();
		}
	};
	template<>
	struct convert<VgMath::Quaternion>
	{
		static Node encode(const VgMath::Quaternion& rhs)
		{
			Node node;
			node.push_back(rhs.im);
			node.push_back(rhs.re);
		}
		static bool decode(const Node& node, VgMath::Quaternion& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.im = node[0].as<VgMath::Vector3>();
			rhs.re = node[1].as<VgMath::Scalar>();
		}
	};
}

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
		out << YAML::Key << "Translate" << YAML::Value << transform.translate;
		out << YAML::Key << "Rotate" << YAML::Value << transform.rotate;
		out << YAML::Key << "Scale" << YAML::Value << transform.scale;
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
		if (ecs.Has<Tag>(entityId))
		{
			out << YAML::Key << "Tag";
			
			out << YAML::BeginMap;
			Tag& currentTag = ecs.Get<Tag>(entityId);
			out << YAML::Key << "Tag name" << YAML::Value << currentTag.tagName;
			out << YAML::EndMap;
		}		
		if (ecs.Has<TransformComponent>(entityId))
		{
			out << YAML::Key << "TransformComponent";

			out << YAML::BeginMap;
			TransformComponent& currentTransformComp = ecs.Get<TransformComponent>(entityId);
			out << YAML::Key << "LocalTransform" << YAML::Value << currentTransformComp.localTransform;
			out << YAML::Key << "Parent" << YAML::Value << currentTransformComp.parent;
			out << YAML::Key << "CumulativeTransform" << YAML::Value << currentTransformComp.cumulatedTransform;
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
			out << YAML::Key << "ParticleEmitter";
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
		LOG_CORE_TRACE("Serializing Scene: {0}", scene->sceneName);

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
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"]) { 
			LOG_CORE_ERROR("Cannot deserialize file: '{0}', missing 'Scene'", filePath);
			return false; 
		}

		std::string sceneName = data["Scene"].as<std::string>();
		LOG_CORE_TRACE("Deserializing Scene: '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities)
		{
			LOG_CORE_ERROR("Cannot deserialize file: '{0}', missing 'Entities'", filePath);
			return false;
		}

		for (auto entity : entities) {
			EntityID entityID = entity["Entity"].as<EntityID>();
			LOG_CORE_TRACE("\tDeserializing Entity: '{0}'", entityID);

			// #TODO
			// For now we assume that at start no gap between EntityID exists
			EntityID deserializedEntity = scene->ecs.CreateEntity();

			if (auto transformCompNode = entity["TransformComponent"]) {
				TransformComponent& transformComp = scene->ecs.Add<TransformComponent>(deserializedEntity);
				if (auto localTransformNode = transformCompNode["LocalTransform"]) {
					transformComp.localTransform.translate = localTransformNode["Translate"].as<VgMath::Vector3>();
					transformComp.localTransform.rotate = localTransformNode["Rotate"].as<VgMath::Quaternion>();
#if ANISOTROPIC_SCALING
					transformComp.m_LocalTransform.scale = localTransformNode["Scale"].as<VgMath::Vector3>();
#else
					transformComp.localTransform.scale = localTransformNode["Scale"].as<VgMath::Scalar>();
#endif
				}
				transformComp.parent = transformCompNode["Parent"].as<EntityID>();
				if (auto cumulativeTransformNode = transformCompNode["CumulativeTransform"])
				{
					transformComp.cumulatedTransform.translate = cumulativeTransformNode["Translate"].as<VgMath::Vector3>();
					transformComp.cumulatedTransform.rotate = cumulativeTransformNode["Rotate"].as<VgMath::Quaternion>();
#if ANISOTROPIC_SCALING
					transformComp.m_CumulatedTransform.scale = cumulativeTransformNode["Scale"].as<VgMath::Vector3>();
#else
					transformComp.cumulatedTransform.scale = cumulativeTransformNode["Scale"].as<VgMath::Scalar>();
#endif
				}

			}
			if (auto staticMeshCompNode = entity["StaticMeshComponent"]) {
				StaticMeshComponent& staticMeshComp = scene->ecs.Add<StaticMeshComponent, TransformComponent>(deserializedEntity);
				staticMeshComp.modelId = staticMeshCompNode["modelId"].as<ModelID>();
				staticMeshComp.materialId = staticMeshCompNode["materialId"].as<MaterialID>();
				staticMeshComp.bCastShadow = staticMeshCompNode["bCastShadow"].as<bool>();
			}
			if (auto particleEmitterNode = entity["ParticleEmitter"]) {
				ParticleEmitter& particleEmitter = scene->ecs.Add<ParticleEmitter, TransformComponent>(deserializedEntity);
				particleEmitter.numberOfParticles = particleEmitterNode["numberOfParticles"].as<uint32_t>();
				particleEmitter.particleToEmitEachTime = particleEmitterNode["particleToEmitEachTime"].as<uint32_t>();
				particleEmitter.lastIndex = particleEmitterNode["lastIndex"].as<uint32_t>();
				if (auto spawningValuesNode = particleEmitterNode["spawningValues"])
				{
					// we need the spawning surface 
					if (auto spawningSurface = spawningValuesNode["spawningSurface"])
					{
						particleEmitter.spawningValues.spawningSurface.m_Size = spawningSurface["m_Size"].as<VgMath::Vector2>();
					}

					particleEmitter.spawningValues.lifeTime = spawningValuesNode["lifeTime"].as<float>();
					particleEmitter.spawningValues.startsize = spawningValuesNode["startsize"].as<float>();
					particleEmitter.spawningValues.endsize = spawningValuesNode["endsize"].as<float>();
					particleEmitter.spawningValues.startSpeed = spawningValuesNode["startSpeed"].as<VgMath::Vector3>();
					particleEmitter.spawningValues.endSpeed = spawningValuesNode["endSpeed"].as<VgMath::Vector3>();
					particleEmitter.spawningValues.startColor = spawningValuesNode["startColor"].as<VgMath::Vector4>();
					particleEmitter.spawningValues.endColor = spawningValuesNode["endColor"].as<VgMath::Vector4>();
					particleEmitter.spawningValues.textureID = spawningValuesNode["textureID"].as<TextureID>();
				}
			}
		}

		return true;
	}

}

}