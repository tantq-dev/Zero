#pragma once
#include "Actor.h"


class Background : public Core::Actor {
	// Inherited via Actor
public:
	Background(std::shared_ptr<Core::World> world);
	void OnUpdate(float dt) override;
	void OnFixedUpdate(float dt) override;
	void OnStart() override;
	void OnDestroy() override;
};
