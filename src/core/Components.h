#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
#include <variant>

namespace Components
{

	/**
	* @brief Transform component that defines an entity's position, scale, and rotation in 2D space.
	*
	* This component is used to represent the spatial properties of an entity:
	* - Position: The entity's location in 2D space
	* - Scale: The entity's size multiplier in both X and Y directions
	* - Rotation: The entity's orientation in radians
	*/
	struct Transform
	{
		Vec2 position = { 0.0f, 0.0f };
		Vec2 scale = { 1.0f, 1.0f };
		float rotation = 0.0f;

		Transform() = default;
		Transform(const Vec2& pos, const Vec2& scl, float rot)
			: position(pos), scale(scl), rotation(rot)
		{
		}
	};

	struct Velocity
	{
		Vec2 velocity = { 0.0f, 0.0f };
		Velocity() = default;
		Velocity(const Vec2& vel)
			: velocity(vel)
		{
		}
	};

	enum class ColliderType
	{
		Circle, Box
	};

	struct BoxCollider
	{
		Vec2 size;
	};

	struct CircleCollider
	{
		float radius;
		Vec2 center(const Vec2& position) const
		{
			return position;
		}
	};

	struct Collider
	{
		ColliderType type;
		std::variant<BoxCollider, CircleCollider> data;
		bool isColliding = false;
		static Collider MakeBox(const Vec2& size)
		{
			return { ColliderType::Box, BoxCollider{size} };
		}

		static Collider MakeCircle(float radius)
		{
			return { ColliderType::Circle, CircleCollider{radius} };
		}

		const BoxCollider* AsBox() const
		{
			return type == ColliderType::Box ? &std::get<BoxCollider>(data) : nullptr;
		}

		const CircleCollider* AsCircle() const
		{
			return type == ColliderType::Circle ? &std::get<CircleCollider>(data) : nullptr;
		}
	};







}