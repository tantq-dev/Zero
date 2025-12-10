#pragma once
#include "entt.hpp"
#include "Components.h"
#include "../core/GameComponents.h"

namespace Game
{
	class EnemySystem
	{
	public:
		EnemySystem() = default;
		~EnemySystem() = default;

		void Update(entt::registry& registry, float deltaTime);
	};
}
