#pragma once
#include "Scene.h"
#include "Components.h"
#include "system/PhysicSystem.h"
#include "system/RenderSystem.h"
#include "system/AnimationSystem.h"
#include "system/InputSystem.h"
#include "system/CameraSystem.h"
// <summary> This is the example scene for the game. </summary>
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
		void HandleInput(SDL_Event& event) override;
		void Render(SDL_Renderer& renderer) override;

	private:
		std::unique_ptr<System::PhysicSystem>		m_physicSystem;
		std::unique_ptr<System::RenderSystem>		m_renderSystem;
		std::unique_ptr<System::AnimationSystem>	m_animationSystem;
		std::unique_ptr<System::InputSystem>		m_inputSystem;
		std::unique_ptr<System::CameraSystem>		m_cameraSystem;


		Components::Tilemap m_tilemap = Components::Tilemap(50, 50, 1000, 1000);
		Vec2 m_lastMousePosition = { 0.0f, 0.0f };
	};
}

