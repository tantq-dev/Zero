#pragma once
#include "Scene.h"
#include "Components.h"
#include "PhysicSystem.h"
#include "RenderSystem.h"
#include <AnimationSystem.h>
namespace Core
{
	class Game;

	class ScenePlay : public Scene
	{
	public:
		ScenePlay() = default;
		~ScenePlay() = default;

		void Initialize(SDL_Renderer& renderer) override;
		void Update(float deltaTime) override;
		void SDoAction() override;
		void Render(SDL_Renderer& renderer) override;

	private:
		std::unique_ptr<System::PhysicSystem> m_physicSystem;
		std::unique_ptr<System::RenderSystem> m_renderSystem;
		std::unique_ptr<System::AnimationSystem> m_animationSystem;
		float m_timeAccumulator = 0.0f;
	};
}

