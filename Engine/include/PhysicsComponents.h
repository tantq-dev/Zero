#pragma once
#include "Vec2.h"
#include "entt.hpp"
#include <variant>

using LayerID = uint8_t;
static constexpr int MAX_LAYERS = 32;
static LayerID DefaultLayer = 0;

enum class ColliderType
{
	Circle, Box
};

namespace Components
{
	struct CollisionPair
	{
		entt::entity a;
		entt::entity b;
	};
	struct Velocity
	{
		Vec2 velocity = { 0.0f, 0.0f };
		Velocity() = default;
		explicit Velocity(const Vec2& vel)
			: velocity(vel)
		{
		}
	};

	

	struct BoxCollider
	{
		Vec2 size;
	};

	struct CircleCollider
	{
		float radius;
	};
	//Physic 

	struct Collider
	{
		ColliderType type;
		std::variant<BoxCollider, CircleCollider> data;
		LayerID layer = DefaultLayer;
		bool isColliding = false;
		bool isTrigger = false;
		static Collider MakeBox(const Vec2& size)
		{
			return { ColliderType::Box, BoxCollider{size} };
		}

		static Collider MakeCircle(float radius)
		{
			return { ColliderType::Circle, CircleCollider{radius} };
		}

		[[nodiscard]] const BoxCollider* AsBox() const
		{
			return type == ColliderType::Box ? &std::get<BoxCollider>(data) : nullptr;
		}

		[[nodiscard]] const CircleCollider* AsCircle() const
		{
			return type == ColliderType::Circle ? &std::get<CircleCollider>(data) : nullptr;
		}
	};
}
