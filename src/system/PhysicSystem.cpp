#include "PhysicSystem.h"
#include <ApplicationConfig.h>
namespace System {
	void PhysicSystem::Update(float deltaTime, entt::registry& registry)
	{
		AABBColisionDetect(registry);
	}
	void PhysicSystem::AABBColisionDetect(entt::registry& registry)
	{
		auto group = registry.group<Components::Transform, Components::BoxCollider, Components::Velocity>();
		for (auto& entity : group)
		{
			auto& transform = group.get<Components::Transform>(entity);
			auto& vel = group.get<Components::Velocity>(entity);
			auto& collider = group.get<Components::BoxCollider>(entity);
			transform.position.x += vel.velocity.x;
			transform.position.y += vel.velocity.y;

			if (transform.position.x + collider.size.x > ApplicationConfig::DEFAULT_WINDOW_WIDTH && vel.velocity.x > 0
				|| transform.position.x < 0 && vel.velocity.x < 0)
			{
				vel.velocity.x = -vel.velocity.x; // Reverse direction
			}

			if (transform.position.y + collider.size.y > ApplicationConfig::DEFAULT_WINDOW_HEIGHT && vel.velocity.y > 0
				|| transform.position.y < 0 && vel.velocity.y < 0)
			{
				vel.velocity.y = -vel.velocity.y; // Reverse direction
			}

			// Check for collisions with other entities
			for (auto& entity2 : group)
			{
				if (entity2 == entity)
				{
					continue; // Skip self
				}
				auto& transform2 = group.get<Components::Transform>(entity2);
				auto& collider2 = group.get<Components::BoxCollider>(entity2);
				auto& vel2 = group.get<Components::Velocity>(entity2);
				bool isColliding1 =
					transform.position.x < transform2.position.x + collider2.size.x &&
					transform.position.y + collider.size.y > transform2.position.y;

				bool isColliding2 =
					transform.position.x + collider.size.x > transform2.position.x &&
					transform.position.y < transform2.position.y + collider2.size.y;

				if (isColliding1 && isColliding2) {
					vel.velocity.x = -vel.velocity.x; // Reverse direction on collision
					vel.velocity.y = -vel.velocity.y; // Reverse direction on collision
					vel2.velocity.x = -vel2.velocity.x; // Reverse direction on collision
					vel2.velocity.y = -vel2.velocity.y; // Reverse direction on collision
				}

			}

		}
	}
	void PhysicSystem::CircleColisionDetect(entt::registry& registry)
	{
		auto group = registry.group<Components::Transform, Components::CircleCollider, Components::Velocity>();
		for (auto& entity : group)
		{
			auto& transform = group.get<Components::Transform>(entity);
			auto& vel = group.get<Components::Velocity>(entity);
			auto& collider = group.get<Components::CircleCollider>(entity);
			transform.position.x += vel.velocity.x;
			transform.position.y += vel.velocity.y;

			if (transform.position.x + collider.radius > ApplicationConfig::DEFAULT_WINDOW_WIDTH && vel.velocity.x > 0
				|| transform.position.x - collider.radius < 0 && vel.velocity.x < 0)
			{
				vel.velocity.x = -vel.velocity.x; // Reverse direction
			}

			if (transform.position.y + collider.radius > ApplicationConfig::DEFAULT_WINDOW_HEIGHT && vel.velocity.y > 0
				|| transform.position.y - collider.radius < 0 && vel.velocity.y < 0)
			{
				vel.velocity.y = -vel.velocity.y; // Reverse direction
			}

			// Check for collisions with other entities
			for (auto& entity2 : group)
			{
				if (entity2 == entity)
				{
					continue; // Skip self
				}
				auto& transform2 = group.get<Components::Transform>(entity2);
				auto& collider2 = group.get<Components::CircleCollider>(entity2);
				auto& vel2 = group.get<Components::Velocity>(entity2);
				bool isColliding1 = collider2.radius + collider.radius >= abs(transform2.position.x - transform.position.x);

				if (isColliding1) {
					vel.velocity.x = -vel.velocity.x; // Reverse direction on collision
					vel.velocity.y = -vel.velocity.y; // Reverse direction on collision
					vel2.velocity.x = -vel2.velocity.x; // Reverse direction on collision
					vel2.velocity.y = -vel2.velocity.y; // Reverse direction on collision
				}

			}

		}
	}
}

