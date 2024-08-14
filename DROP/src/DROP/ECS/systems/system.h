#pragma once

#include "DROP/ECS/entity.h"
#include "set"

// We will use OOP only here, since system will be of a reduced number
// in comparison with Entities or Components
// And they hold some logic meaning
class System
{
public:
	std::set<Entity> mEntities;

	virtual void Start() {};
	virtual void Update(float deltaTime) {};
	virtual void FixedUpdate(float fixedDeltaTime) {};
	virtual void End() {};
};