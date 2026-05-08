#pragma once
#include "Actor.h"
#include "Logger.h"
#include "World.h"
#include "Components.h"
class Player : public Core::Actor {
public:
	Player(std::shared_ptr<Core::World> world)
		: Actor(world)
	{
		LOG_INFO("Create entity");
	}
	void OnUpdate(float dt) override {
		LOG_INFO("Player update");
	};
	void OnFixedUpdate(float dt) override {
	
	};
	void OnStart() override {
		AddComponent<Components::Transform2D>();
	};
	void OnDestroy()override {
	
	};
};