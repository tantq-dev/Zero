#pragma once
#include "Scene.h"
#include "Component.h"
#include "Game.h"


namespace Core
{
	class ScenePlay : public Scene
	{
	public:
		ScenePlay() = default;
		~ScenePlay() = default;
		void Initialize();
		void Update(float deltaTime) override;
		void SRender(SDL_Renderer* render) override;
		void SDoAction() override;
	};
}

