#pragma once
#include "core/Scene.h"
#include "system/RenderSystem.h"
#include "system/InputSystem.h"
#include "system/CameraSystem.h"

#include "HeightMap.h"
#include "entt.hpp"
namespace Tool
{
	// Forward declarations
	class Game;

	class PathPlacingTool : public Core::Scene
	{
	public:
		PathPlacingTool() = default;
		~PathPlacingTool() = default;
		void InitializePathPlacement();
		void UpdatePathPlacement(float deltaTime);
		void Initialize(SDL_Renderer& renderer) override;
		void Update(float deltaTime) override;
		void HandleInput(SDL_Event& event) override;
		void Render(SDL_Renderer& renderer) override;
		void HandleUI(SDL_Event& event) override;

	private:
		std::unique_ptr<System::RenderSystem> m_renderSystem;
		std::unique_ptr<System::InputSystem> m_inputSystem;
		std::unique_ptr<System::CameraSystem> m_cameraSystem;
		HeightMap m_heightMap;
		entt::registry m_registry;
	};

}
