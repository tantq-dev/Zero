#pragma once
#include "entt.hpp"
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


