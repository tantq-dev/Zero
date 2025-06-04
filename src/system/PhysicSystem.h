#pragma once
#include "entt.hpp"
#include "core/Components.h"

namespace System
{
	class PhysicSystem
	{
	public:
		PhysicSystem() = default;
		~PhysicSystem() = default;
		void Update(float deltaTime, entt::registry& registry);
	private:
		void CollisionDetect(entt::registry& registry);
		bool CheckAABBCollision(
			const Components::Transform& t1, const Components::BoxCollider& b1,
			const Components::Transform& t2, const Components::BoxCollider& b2);
		bool CheckCircleCollision(
			const Components::Transform& t1, const Components::CircleCollider& c1,
			const Components::Transform& t2, const Components::CircleCollider& c2);
		bool CheckCircleAABBCollision(
			const Components::Transform& circleTransform,
			const Components::CircleCollider& circleCollider,
			const Components::Transform& rectTransform,
			const Components::BoxCollider& rectCollider);
		bool CheckCircleCollisionEdge(
			const Components::Transform& circleTransform,
			const Components::CircleCollider& circleCollider,
			const Components::Velocity& vel);
		bool CheckBoxCollisionEdge(
			const Components::Transform& boxTransform,
			const Components::BoxCollider& boxCollider,
			const Components::Velocity& vel);
	};


}


