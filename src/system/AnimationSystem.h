#pragma once
#include "entt.hpp"
#include "Components.h"
namespace System
{
	class AnimationSystem
	{
	
	public:
			AnimationSystem() = default;
		~AnimationSystem() = default;

		void Update(entt::registry& registry, const float& dt);
	};
}


