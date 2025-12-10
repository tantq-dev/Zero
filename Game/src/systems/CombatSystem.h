#pragma once
#include "entt.hpp"
#include "Components.h"
#include "../core/GameComponents.h"

namespace Game
{
	class CombatSystem
	{
	public:
		CombatSystem() = default;
		~CombatSystem() = default;

		void Update(entt::registry& registry, float deltaTime);
	};
}
