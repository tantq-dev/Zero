//#include "ScenePlay.h"
//#include "config/ApplicationConfig.h"
//#include <random>
//#include "resources/ResourcesManager.h"
//#include "utilities/Logger.h"
//
//// Helper function to generate a random float in a given range
//float RandomFloat(float min, float max) {
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_real_distribution<float> dis(min, max);
//	return dis(gen);
//}
//
//void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius)
//{
//	for (int y = -radius; y <= radius; ++y)
//	{
//		for (int x = -radius; x <= radius; ++x)
//		{
//			if (x * x + y * y <= radius * radius)
//			{
//				SDL_RenderPoint(renderer, centerX + x, centerY + y);
//			}
//		}
//	}
//}
//
//namespace Core
//{
//	void ScenePlay::Initialize(SDL_Renderer& renderer)
//	{
//
//		m_physicSystem = std::make_unique<System::PhysicSystem>();
//		m_renderSystem = std::make_unique<System::RenderSystem>();
//		m_animationSystem = std::make_unique<System::AnimationSystem>();
//		m_inputSystem = std::make_unique<System::InputSystem>();
//		m_cameraSystem = std::make_unique<System::CameraSystem>();
//		m_gridSystem = std::make_unique<System::GridSystem>(Components::Grid(30, 30, 24, 12));
//
//
//		Components::InputAction actionSpace("Test");
//		Components::InputAction actionMouse("Test_Mouse");
//		Components::InputAction actionMouseMotion("Test_Mouse_Motion");
//		Components::InputAction actionMouseWheel("Test_Mouse_Wheel");
//
//		Components::InputAction actionMiddleMouse("Test_Mouse_Middle");
//
//		actionSpace.AddBinding(SDL_SCANCODE_SPACE);
//		actionMouse.AddMouseButtonBinding(SDL_BUTTON_LEFT);
//		actionMouseMotion.AddMouseMotionBinding();
//		actionMouseWheel.AddMouseWheelBinding();
//		actionMiddleMouse.AddMouseButtonBinding(SDL_BUTTON_MIDDLE);
//
//		m_inputSystem->RegisterAction(actionSpace);
//		m_inputSystem->RegisterAction(actionMouse);
//		m_inputSystem->RegisterAction(actionMouseMotion);
//		m_inputSystem->RegisterAction(actionMouseWheel);
//		m_inputSystem->RegisterAction(actionMiddleMouse);
//
//
//		Components::Camera camera;
//		m_cameraSystem->AddCamera("Main", camera);
//		m_cameraSystem->SetCurrentCamera("Main");
//
//	}
//
//
//	void ScenePlay::Update(const float deltaTime)
//	{
//		if (m_inputSystem->IsActionHeld("Test_Mouse_Middle")) {
//			Vec2 mouseDelta = m_inputSystem->GetMousePosition("Test_Mouse_Motion") - m_lastMousePosition;
//			m_lastMousePosition = m_inputSystem->GetMousePosition("Test_Mouse_Motion");
//
//			if (mouseDelta == m_inputSystem->GetMousePosition("Test_Mouse_Motion")) {
//				return; // Skip the first frame to avoid a large jump
//			}
//			m_cameraSystem->AdjustCameraPosition(mouseDelta);
//		}
//		else {
//			m_lastMousePosition = Vec2::zero();
//		}
//
//		if (m_inputSystem->IsActionPressed("Test_Mouse"))
//		{
//			Vec2 mousePos = m_inputSystem->GetMousePosition("Test_Mouse_Motion");
//			Vec2 mouseWorld = { 0,0 };
//			m_cameraSystem->CameraViewToWorld(mousePos, mouseWorld);
//			auto& tile = m_gridSystem->GetCell(mouseWorld);
//			tile.isColor = !tile.isColor;
//			LOG_INFO("Tile at position" + std::to_string(tile.position.x) + " " + std::to_string(tile.position.y));
//		}
//
//
//		m_cameraSystem->AdjustCameraZoom(m_inputSystem->GetMouseWheelDelta("Test_Mouse_Wheel"));
//
//		m_inputSystem->ResetMouseWheelDelta("Test_Mouse_Wheel");
//		m_inputSystem->ResetMousePress("Test_Mouse");
//		m_inputSystem->ResetMousePress("Test_Mouse_Middle");
//
//	}
//	void ScenePlay::HandleInput(SDL_Event& event)
//	{
//		m_inputSystem->HandleInput(event);
//	}
//	void ScenePlay::Render(SDL_Renderer& renderer)
//	{
//		m_renderSystem->RenderGrid(m_gridSystem->GetTileSheets(), renderer, *m_cameraSystem);
//		m_renderSystem->Render(m_Registry, renderer);
//	}
//}