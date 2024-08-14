#pragma once

extern ECS g_ECS;

class TestSystem : public System
{
	virtual void Update(float deltaTime) override
	{
		g_ECS.AddEntity();
	}
};