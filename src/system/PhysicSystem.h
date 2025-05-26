#pragma once
#include "entt.hpp"
#include "Components.h"

namespace System
{
	class PhysicSystem
	{
	public:
		PhysicSystem() = default;
		~PhysicSystem() = default;
		void Update(float deltaTime, entt::registry& registry);
	private:
		void AABBColisionDetect(entt::registry& registry);
		void CircleColisionDetect(entt::registry& registry);
	};


}


