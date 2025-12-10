#pragma once
#include "Scene.h"
#include "core/Grid.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include <player/PlayerMovementSystem.h>
#include <core/AnimationSys.h>
namespace Game
{
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
		Game::PlayerMovementSystem m_movementSystem;
		Game::AnimationSys m_animationSystem;
		System::InputSystem m_inputSystem;
		System::CameraSystem m_cameraSystem;
		entt::entity m_playerEntity;
		entt::entity m_cameraEntity;

		void CreatePlayer();
		void CreateCamera();
		void CreateEnemy(Vec2 position);
	};
}
