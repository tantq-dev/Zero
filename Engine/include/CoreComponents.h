#pragma once
#include "Vec2.h"

namespace Components
{
	struct Transform2D
	{
		Vec2 position = { 0.0f, 0.0f };
		Vec2 scale = { 1.0f, 1.0f };
		float rotation = 0.0f;

		Transform2D() = default;
		Transform2D(const Vec2& pos, const Vec2& scl, float rot)
			: position(pos), scale(scl), rotation(rot)
		{
		}
		Transform2D(const Vec2& pos, const Vec2& s) :position(pos), scale(s) {

		}
	};
}
