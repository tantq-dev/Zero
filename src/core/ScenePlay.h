#pragma once
#include "Scene.h"
#include "Components.h"
#include "PhysicSystem.h"

namespace Core
{
	class Game;

	class ScenePlay : public Scene
	{
	public:
		ScenePlay() = default;
		~ScenePlay() = default;

		void Initialize() override;
		void Update(float deltaTime) override;
		void SRender(SDL_Renderer* renderer) override;
		void SDoAction() override;

	private:
		std::unique_ptr<System::PhysicSystem> physicSystem;
	};
}

