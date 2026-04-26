#pragma once
#include "entt.hpp"
#include "Components.h"

struct PlayerTag {

};

entt::entity CreatePlayer(entt::registry& reg)
{
	auto e = reg.create();

	reg.emplace<Components::Transform2D>(e, Vec2{ 0,0 }, Vec2{ 1,1 });
	reg.emplace<PlayerTag>(e);
	reg.emplace<Components::Shape>(e, Components::Shape{
	.type = Components::Shape::Type::Rect,
	.color = {0,255,255,255},
	.size = {32, 32}
		});
	return e;
}