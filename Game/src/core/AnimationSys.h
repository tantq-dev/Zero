#pragma once
#include "entt.hpp"
#include "Components.h"
namespace Game
{
	class AnimationSys
	{
	public:
		AnimationSys() = default;
		~AnimationSys() = default;
		void Update(entt::registry& registry, const float& dt);
	private:
	};
}