#pragma once
#include "Scene.h"
class WorldScene : public Core::Scene
{
public:
	WorldScene() = default;

	void Initialize(SDL_Renderer& renderer) override;
	void Update(const double& deltaTime) override;
	void FixedUpdate(const double& deltaTime) override;
	void Render(SDL_Renderer& renderer, const double& alpha) override;
	void HandleInput(SDL_Event& event) override;
	void HandleUI(SDL_Event& event) override;
};
