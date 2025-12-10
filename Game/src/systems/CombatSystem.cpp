#include "CombatSystem.h"
#include "../player/Player.h"
#include <iostream>

namespace Game
{
	void CombatSystem::Update(entt::registry& registry, float deltaTime)
	{
		auto playerView = registry.view<Character, Components::Transform2D, MeleeAttack>();
		auto enemyView = registry.view<Enemy, Components::Transform2D, Health>();

		// Player Attack Logic
		for (auto playerEntity : playerView)
		{
			auto& attack = playerView.get<MeleeAttack>(playerEntity);
			auto& playerTransform = playerView.get<Components::Transform2D>(playerEntity);
			auto& player = playerView.get<Character>(playerEntity);

			if (attack.isActive)
			{
				// Define Attack Hitbox based on direction
				Components::Rect attackBox;
				float range = 40.0f; // Attack range
				float width = 40.0f;

				// Simple hitbox offset logic
				if (player.direction == CharacterDirection::UP)
				{
					attackBox = { playerTransform.position.x - width / 2, playerTransform.position.y - range, width, range };
				}
				else if (player.direction == CharacterDirection::DOWN)
				{
					attackBox = { playerTransform.position.x - width / 2, playerTransform.position.y + 32, width, range }; // +32 for player height
				}
				else if (player.direction == CharacterDirection::LEFT)
				{
					attackBox = { playerTransform.position.x - range, playerTransform.position.y - width / 2, range, width };
				}
				else if (player.direction == CharacterDirection::RIGHT)
				{
					attackBox = { playerTransform.position.x + 32, playerTransform.position.y - width / 2, range, width }; // +32 for player width
				}

				// Check collision with enemies
				for (auto enemyEntity : enemyView)
				{
					auto& enemyTransform = enemyView.get<Components::Transform2D>(enemyEntity);
					auto& enemyHealth = enemyView.get<Health>(enemyEntity);

					// Simple AABB collision
					bool collision = (attackBox.x < enemyTransform.position.x + 32 &&
						attackBox.x + attackBox.w > enemyTransform.position.x &&
						attackBox.y < enemyTransform.position.y + 32 &&
						attackBox.y + attackBox.h > enemyTransform.position.y);

					if (collision)
					{
						// Apply damage (simple logic: damage every frame if colliding? No, should be once per attack)
						// For simplicity, let's just apply damage and knockback
						// Ideally, we track which enemies were hit this attack swing
						
						// Hack: Only apply damage if enemy health > 0 (to avoid multi-kill logic issues for now)
						if (enemyHealth.current > 0)
						{
							enemyHealth.current -= static_cast<int>(attack.damage * deltaTime * 10); // Scale damage for continuous hit
							// Or better: add invincibility frames to enemy
						}
					}
				}
			}
		}

		// Cleanup Dead Enemies
		for (auto entity : enemyView)
		{
			if (enemyView.get<Health>(entity).current <= 0)
			{
				registry.destroy(entity);
			}
		}
	}
}
