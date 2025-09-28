#pragma once
#include "entt.hpp"
#include "Components.h"
#include <Window.h>

namespace System
{
	class PhysicSystem
	{
	public:
		PhysicSystem() = default;
		~PhysicSystem() = default;
		void Update(const float& deltaTime,  entt::registry& registry);
	private:
		void CollisionDetect( entt::registry& registry);
		bool CheckAABBCollision(
			const Components::Transform2D& t1, const Components::BoxCollider& b1,
			const Components::Transform2D& t2, const Components::BoxCollider& b2);
		bool CheckCircleCollision(
			const Components::Transform2D& t1, const Components::CircleCollider& c1,
			const Components::Transform2D& t2, const Components::CircleCollider& c2);
		bool CheckCircleAABBCollision(
			const Components::Transform2D& circleTransform,
			const Components::CircleCollider& circleCollider,
			const Components::Transform2D& rectTransform,
			const Components::BoxCollider& rectCollider);
	};


}


