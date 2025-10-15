#pragma once
#include "Scene.h"
#include "core/Grid.h"
class WorldScene : public Core::Scene
{
public:
	WorldScene() = default;

	void Initialize() override;
	void Update(const double& deltaTime) override;
	void FixedUpdate(const double& deltaTime) override;
	void HandleInput(SDL_Event& event) override;
	void HandleUI(SDL_Event& event) override;
private:
	Grid m_grid;
};
