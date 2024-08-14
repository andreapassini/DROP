#pragma once

#include "unordered_map"
#include "string"
#include "memory"
#include "queue"
#include "assert.h"

#include "entity.h"
#include "components/component.h"
#include "systems/system.h"


// Since we use templates, put functions in .h

class IComponentPool
{
public:
	virtual ~IComponentPool() = default;
};

template <typename T>
class ComponentPool : public IComponentPool
{
public:
	ComponentPool(uint32_t reserveSize)
	{
		m_Pool.reserve(reserveSize);
	}
	void InsertData(Entity entity, T component)
	{
		if (m_Pool.size() < entity)
			m_Pool.resize(entity);

		m_Pool.emplace(m_Pool.begin() + entity, component);
	}
	T& InsertData(Entity entity)
	{
		if (m_Pool.size() < entity)
			m_Pool.resize(entity);

		auto iter = m_Pool.emplace(m_Pool.begin() + entity);
		return *iter;
	}

	void RemoveData(Entity entity)
	{
		assert(m_Pool.size() < entity, "Removing non-existent component");

		m_Pool.erase(m_Pool.begin() + entity);
	}
	T& GetData(Entity entity)
	{
		assert(m_Pool.size() < entity, "Retrieving non-existent component");

		return m_Pool[entity];
	}
private:
	std::vector<T> m_Pool;
};

class ECS
{
public:
	ECS(
		const uint32_t reserveSize,
		const uint32_t maxEnities,
		const uint32_t maxComponents
	)
		: m_Reserve(reserveSize),
		m_MaxEntities(maxEnities),
		m_MaxComponents(maxComponents)
	{ }
	~ECS() {};

	// - Entity
	Entity AddEntity()
	{
		assert(m_EntityCount < m_MaxEntities, "Too many entities.");

		// Consider using ZII: zero as initialization
		// At entity 0, create a stub(empty entity)

		Entity id = 0;

		if (!m_ReusableEntities.empty())
		{
			id = m_ReusableEntities.front();
			m_ReusableEntities.pop();
		}
		else
		{
			if (m_EntityCount > 0)
				id = m_EntityCount - 1;
		}

		m_EntityCount++;

		return id;
	}
	void RemoveEntity(Entity entity)
	{
		assert(entity < m_MaxEntities, "Entity out of range.");

		m_ReusableEntities.push(entity);
		m_EntityCount--;

		return;
	}


	// - Component
	template <typename T>
	void RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Registering component type more than once.");

		m_ComponentPools.insert({ typeName, std::make_shared<ComponentPool<T>>(m_Reserve) });
		m_ComponentTypes.insert({ typeName, m_ComponentCount });
		m_ComponentCount++;
	}

	template <typename T>
	ComponentType GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(), "Component not registered before use.");

		// Return this component's type - used for creating signatures
		return m_ComponentTypes[typeName];
	}

	template <typename T>
	std::shared_ptr<ComponentPool<T>> GetComponentPool()
	{
		const char* typeName = typeid(T).name();

		assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(), "Component not registered before use.");

		return std::static_pointer_cast<ComponentPool<T>>(m_ComponentPools[typeName]);
	}

	template <typename T>
	void AddComponent(Entity entity, T component)
	{
		// Add a component to the array for an entity
		GetComponentPool<T>()->InsertData(entity, component);
	}

	template <typename T>
	T& AddComponent(Entity entity)
	{
		// Add a component to the array for an entity
		return GetComponentPool<T>()->InsertData(entity);
	}

	template <typename T>
	void RemoveComponent(Entity entity)
	{
		// Remove a component from the array for an entity
		GetComponentPool<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity)
	{
		// Get a reference to a component from the array for an entity
		return GetComponentPool<T>()->GetData(entity);
	}


	// - System
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		// Create a pointer to the system and return it so it can be used externally
		auto system = std::make_shared<T>();
		m_Systems.emplace_back(system);
		return system;
	}
	void SystemsStart()
	{
		for (size_t i = 0; i < m_Systems.size(); i++)
		{
			m_Systems[i]->Start();
		}
	}
	void SystemsUpdate(float deltaTime)
	{
		for (size_t i = 0; i < m_Systems.size(); i++)
		{
			m_Systems[i]->Update(deltaTime);
		}

	}
	void SystemsFixedUpdate(float fixedDeltaTime)
	{
		for (size_t i = 0; i < m_Systems.size(); i++)
		{
			m_Systems[i]->FixedUpdate(fixedDeltaTime);
		}

	}
	void SystemsEnd()
	{
		for (size_t i = 0; i < m_Systems.size(); i++)
		{
			m_Systems[i]->End();
		}

	}

private:
	uint32_t m_Reserve = 0;

	// Entity
	const Entity m_MaxEntities = 5'000;
	std::queue<Entity> m_ReusableEntities{}; // The entities hole, the one that have been deleted that will be reused
	Entity m_EntityCount = 0;

	// Component
	const ComponentType m_MaxComponents = 5'000;
	ComponentType m_ComponentCount = 0;
	// Map from type string pointer to a component type
	std::unordered_map<const char*, ComponentType> m_ComponentTypes{};
	std::unordered_map<const char*, std::shared_ptr<IComponentPool>> m_ComponentPools{};

	// System
	// Map from system type string pointer to a system pointer
	std::vector<std::shared_ptr<System>> m_Systems{};

};