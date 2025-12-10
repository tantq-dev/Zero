#include "EnemySystem.h"
#include "../player/Player.h"

namespace Game
{
	void EnemySystem::Update(entt::registry& registry, float deltaTime)
	{
		auto enemyView = registry.group<>(entt::get<Enemy, Components::Transform2D, Components::Velocity>);
		auto playerView = registry.view<Character, Components::Transform2D>();

		// Find player position (assuming single player)
		Components::Transform2D playerTransform;
		bool playerFound = false;
		for (auto entity : playerView)
		{
			playerTransform = playerView.get<Components::Transform2D>(entity);
			playerFound = true;
			break;
		}

		if (!playerFound) return;

		for (auto entity : enemyView)
		{
			auto& enemy = enemyView.get<Enemy>(entity);
			auto& transform = enemyView.get<Components::Transform2D>(entity);
			auto& velocity = enemyView.get<Components::Velocity>(entity);

			// Calculate distance to player
			float dx = playerTransform.position.x - transform.position.x;
			float dy = playerTransform.position.y - transform.position.y;
			float distance = std::sqrt(dx * dx + dy * dy);

			velocity.velocity = { 0.0f, 0.0f };

			if (distance <= enemy.detectRange && distance > enemy.attackRange)
			{
				enemy.isChasing = true;
				// Normalize direction
				float length = std::sqrt(dx * dx + dy * dy);
				if (length > 0)
				{
					velocity.velocity.x = (dx / length) * enemy.moveSpeed;
					velocity.velocity.y = (dy / length) * enemy.moveSpeed;
				}
			}
			else
			{
				enemy.isChasing = false;
			}

			// Apply velocity
			transform.position.x += velocity.velocity.x * deltaTime;
			transform.position.y += velocity.velocity.y * deltaTime;
		}
	}
}
