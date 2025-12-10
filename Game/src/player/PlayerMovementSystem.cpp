#include "PlayerMovementSystem.h"
#include "Player.h"
#include "../core/GameComponents.h"

namespace Game
{
	void PlayerMovementSystem::Update(entt::registry& registry, System::InputSystem& inputSystem, float deltaTime)
	{
		UpdatePlayerMovement(registry, inputSystem, deltaTime);
	}

	void PlayerMovementSystem::UpdatePlayerMovement(entt::registry& registry, System::InputSystem& inputSystem, float deltaTime)
	{
		auto view = registry.group<>(entt::get<Character, Components::Transform2D, Components::Velocity>);

		for (auto entity : view)
		{
			auto& player = view.get<Character>(entity);
			player.justSwitchedDirection = false;
			player.justSwitchedState = false;
			auto& transform = view.get<Components::Transform2D>(entity);
			auto& velocity = view.get<Components::Velocity>(entity);

			// Handle Attack
			MeleeAttack* attack = registry.try_get<MeleeAttack>(entity);
			if (attack)
			{
				// Cooldown management
				if (attack->timeSinceLastAttack < attack->cooldown)
				{
					attack->timeSinceLastAttack += deltaTime;
				}

				// Attack Input
				if (inputSystem.IsActionPressed("Attack") && attack->timeSinceLastAttack >= attack->cooldown && !attack->isActive)
				{
					attack->isActive = true;
					attack->timeActive = 0.0f;
					attack->timeSinceLastAttack = 0.0f;
					player.state = CharacterState::ATTACKING;
					player.justSwitchedState = true;
					
					// Stop movement when attacking
					velocity.velocity = { 0.0f, 0.0f };
				}

				// Attack Duration
				if (attack->isActive)
				{
					attack->timeActive += deltaTime;
					if (attack->timeActive >= attack->duration)
					{
						attack->isActive = false;
						player.state = CharacterState::IDLE;
						player.justSwitchedState = true;
					}
					else
					{
						// Lock movement
						velocity.velocity = { 0.0f, 0.0f };
						return; // Skip movement logic
					}
				}
			}

			// Reset velocity
			velocity.velocity = { 0.0f, 0.0f };

			if (inputSystem.IsActionPressed("MoveUp")
				|| inputSystem.IsActionPressed("MoveDown") 
				|| inputSystem.IsActionPressed("MoveLeft")
				|| inputSystem.IsActionPressed("MoveRight"))
			{
				player.justSwitchedDirection = true;
				player.justSwitchedState = true;
				
			}

			// Check input and update velocity
			if (inputSystem.IsActionHeld("MoveUp"))
			{
				player.direction = CharacterDirection::UP;
				player.state = CharacterState::MOVING;
				velocity.velocity.y = -player.moveSpeed;
			}
			if (inputSystem.IsActionHeld("MoveDown"))
			{
				player.direction = CharacterDirection::DOWN;
				player.state = CharacterState::MOVING;	
				velocity.velocity.y = player.moveSpeed;
			}
			if (inputSystem.IsActionHeld("MoveLeft"))
			{
				player.direction = CharacterDirection::LEFT;
				player.state = CharacterState::MOVING;

				velocity.velocity.x = -player.moveSpeed;
			}
			if (inputSystem.IsActionHeld("MoveRight"))
			{
				player.direction = CharacterDirection::RIGHT;
				player.state = CharacterState::MOVING;

				velocity.velocity.x = player.moveSpeed;
			}

			// Normalize diagonal movement
			if (velocity.velocity.x != 0.0f && velocity.velocity.y != 0.0f)
			{
				float length = sqrt(velocity.velocity.x * velocity.velocity.x + velocity.velocity.y * velocity.velocity.y);
				velocity.velocity.x = (velocity.velocity.x / length) * player.moveSpeed;
				velocity.velocity.y = (velocity.velocity.y / length) * player.moveSpeed;
			}
			
			// If no movement input and not attacking, set to IDLE
			if (velocity.velocity.x == 0.0f && velocity.velocity.y == 0.0f && player.state == CharacterState::MOVING)
			{
				player.state = CharacterState::IDLE;
				player.justSwitchedState = true;
			}

			// Apply velocity to position
			transform.position.x += velocity.velocity.x * deltaTime;
			transform.position.y += velocity.velocity.y * deltaTime;
		}
	}
}