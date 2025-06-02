#include "PhysicSystem.h"
#include <ApplicationConfig.h>
namespace System {
	void PhysicSystem::Update(float deltaTime, entt::registry& registry)
	{
		CollisionDetect(registry);
	}

	void PhysicSystem::CollisionDetect(entt::registry& registry)
	{
		auto group = registry.group(entt::get<Components::Transform, Components::Collider, Components::Velocity>);
		
		// Reset collision flags first
		for (auto entity : group)
		{
			auto& collider = group.get<Components::Collider>(entity);
			auto& transform = group.get<Components::Transform>(entity);
			auto& velocity = group.get<Components::Velocity>(entity);
			collider.isColliding = false;

			if (collider.type == Components::ColliderType::Box)
			{
				collider.isColliding = CheckBoxCollisionEdge(transform ,*collider.AsBox(), velocity);
			}
			else if (collider.type == Components::ColliderType::Circle)
			{
				collider.isColliding = CheckCircleCollisionEdge(transform, *collider.AsCircle(), velocity);
			}
		}
		
		// Check collisions - only check each pair once
		for (auto it1 = group.begin(); it1 != group.end(); ++it1)
		{
			auto entity1 = *it1;
			auto [transform1, collider1] = group.get<Components::Transform, Components::Collider>(entity1);
			
			// Start from the next entity to avoid duplicate checks
			for (auto it2 = it1 + 1; it2 != group.end(); ++it2)
			{
				auto entity2 = *it2;
				auto [transform2, collider2] = group.get<Components::Transform, Components::Collider>(entity2);
				
				bool collision = false;
				
				// Check collision based on collider types
				if (collider1.type == Components::ColliderType::Box && collider2.type == Components::ColliderType::Box)
				{
					collision = CheckAABBCollision(transform1, *collider1.AsBox(), transform2, *collider2.AsBox());
				}
				else if (collider1.type == Components::ColliderType::Circle && collider2.type == Components::ColliderType::Circle)
				{
					collision = CheckCircleCollision(transform1, *collider1.AsCircle(), transform2, *collider2.AsCircle());
				}
				else if (collider1.type == Components::ColliderType::Circle && collider2.type == Components::ColliderType::Box)
				{
					collision = CheckCircleAABBCollision(transform1, *collider1.AsCircle(), transform2, *collider2.AsBox());
				}
				else if (collider1.type == Components::ColliderType::Box && collider2.type == Components::ColliderType::Circle)
				{
					collision = CheckCircleAABBCollision(transform2, *collider2.AsCircle(), transform1, *collider1.AsBox());
				}
				
				// Only set flags if a collision was detected
				if (collision)
				{
					collider1.isColliding = true;
					collider2.isColliding = true;
				}
			}
		}
	}

	bool PhysicSystem::CheckAABBCollision(const Components::Transform& t1,
		const Components::BoxCollider& b1, 
		const Components::Transform& t2, 
		const Components::BoxCollider& b2)
	{
		return	t1.position.x < t2.position.x + b2.size.x &&
				t1.position.y + b1.size.y > t2.position.y && 
				t1.position.x + b1.size.x > t2.position.x &&
				t1.position.y < t2.position.y + b2.size.y;

	}
	bool PhysicSystem::CheckCircleCollision(const Components::Transform& t1, 
		const Components::CircleCollider& c1, 
		const Components::Transform& t2, 
		const Components::CircleCollider& c2)
	{
		float radiusSum = c1.radius + c2.radius;
		return radiusSum >= abs(t1.position.x - t2.position.x) && radiusSum >= abs(t1.position.y - t2.position.y);
		
	}
	bool PhysicSystem::CheckCircleAABBCollision(const Components::Transform& circleTransform, 
		const Components::CircleCollider& circleCollider,
		const Components::Transform& rectTransform,
		const Components::BoxCollider& rectCollider)
	{

		return circleTransform.position.x + circleCollider.radius > rectTransform.position.x - rectCollider.size.x / 2 &&
			circleTransform.position.x - circleCollider.radius < rectTransform.position.x + rectCollider.size.x / 2 &&
			circleTransform.position.y + circleCollider.radius > rectTransform.position.y - rectCollider.size.y / 2 &&
			circleTransform.position.y - circleCollider.radius < rectTransform.position.y + rectCollider.size.y / 2;;
	}
	bool PhysicSystem::CheckCircleCollisionEdge(const Components::Transform& circleTransform, const Components::CircleCollider& circleCollider, const Components::Velocity& vel)
	{
		return circleTransform.position.x + circleCollider.radius > ApplicationConfig::DEFAULT_WINDOW_WIDTH && vel.velocity.x > 0 ||
			circleTransform.position.x - circleCollider.radius < 0 && vel.velocity.x < 0 ||
			circleTransform.position.y + circleCollider.radius > ApplicationConfig::DEFAULT_WINDOW_HEIGHT &&vel.velocity.y > 0 ||
			circleTransform.position.y - circleCollider.radius < 0 && vel.velocity.y < 0;
	}
	bool PhysicSystem::CheckBoxCollisionEdge(const Components::Transform& boxTransform, const Components::BoxCollider& boxCollider, const Components::Velocity& vel)
	{
		return boxTransform.position.x + boxCollider.size.x > ApplicationConfig::DEFAULT_WINDOW_WIDTH && vel.velocity.x > 0 ||
			boxTransform.position.x - boxCollider.size.x < 0 && vel.velocity.x < 0 ||
			boxTransform.position.y + boxCollider.size.y > ApplicationConfig::DEFAULT_WINDOW_HEIGHT && vel.velocity.y > 0 ||
			boxTransform.position.y - boxCollider.size.y < 0 && vel.velocity.y < 0;
	}
}

