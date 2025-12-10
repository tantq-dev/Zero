#pragma once
#include "entt.hpp"
#include "Components.h"
#include "InputSystem.h"

namespace Game
{
	class PlayerMovementSystem
	{
	public:
		PlayerMovementSystem() = default;
		~PlayerMovementSystem() = default;

		void Update(entt::registry& registry, System::InputSystem& inputSystem, float deltaTime);

	private:
		void UpdatePlayerMovement(entt::registry& registry, System::InputSystem& inputSystem, float deltaTime);
	};

}