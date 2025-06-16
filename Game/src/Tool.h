#pragma once
#include "core/Scene.h"
#include <system/PhysicSystem.h>
#include <system/RenderSystem.h>
#include <system/AnimationSystem.h>
#include <system/InputSystem.h>
#include <system/GridSystem.h>
#include "UIManager.h"
#include "MapEditor.h"
#include "DataHandler.h"

namespace Tool
{
	class Tool : public Core::Scene {

	public:
		Tool() = default;
		~Tool() = default;

		void Initialize(SDL_Renderer& renderer) override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer& renderer) override;
		void HandleInput(SDL_Event& event) override;
		void HandleUI(SDL_Event& event) override;
	private:
		std::unique_ptr<System::PhysicSystem>		m_physicSystem;
		std::unique_ptr<System::RenderSystem>		m_renderSystem;
		std::unique_ptr<System::AnimationSystem>	m_animationSystem;
		std::unique_ptr<System::InputSystem>		m_inputSystem;
		std::unique_ptr<System::CameraSystem>		m_cameraSystem;
		std::unique_ptr<System::GridSystem>			m_gridSystem;
		std::unique_ptr<UI::UIManager>				m_uiManager;
		std::unique_ptr<MapEditor>					m_mapEditor;
		std::unique_ptr<DataHandler>				m_dataHandler;

		Vec2 m_lastMousePosition = { 0.0f, 0.0f };

	};
}