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

	struct BoxCollider {
		Vec2 size;
		BoxCollider() = default;
		BoxCollider(const Vec2& sz)
			: size(sz)
		{
		}
	};

	struct CircleCollider {
		float radius;
		Vec2 offset;

		Vec2 center(const Vec2& position) const {
			return position + offset;
		}
	};

}