#pragma once
#include "Scene.h"
class MainMenuScene : public Core::Scene
{
public:
	MainMenuScene() = default;
	void Initialize(SDL_Renderer& renderer) override;
	void Update(const double& deltaTime) override;
	void FixedUpdate(const double& deltaTime) override;
	void Render(SDL_Renderer& renderer, const double& alpha) override;
	void HandleInput(SDL_Event& event) override;
	void HandleUI(SDL_Event& event) override;
};