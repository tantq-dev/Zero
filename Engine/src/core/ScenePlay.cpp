#include "ScenePlay.h"
#include "config/ApplicationConfig.h"
#include <random>
#include "resources/ResourcesManager.h"
#include "utilities/Logger.h"


// Helper function to generate a random float in a given range
float RandomFloat(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius)
{
	for (int y = -radius; y <= radius; ++y)
	{
		for (int x = -radius; x <= radius; ++x)
		{
			if (x * x + y * y <= radius * radius)
			{
				SDL_RenderPoint(renderer, centerX + x, centerY + y);
			}
		}
	}
}

namespace Core
{
	void ScenePlay::Initialize(SDL_Renderer& renderer)
	{

		m_physicSystem = std::make_unique<System::PhysicSystem>();
		m_renderSystem = std::make_unique<System::RenderSystem>();
		m_animationSystem = std::make_unique<System::AnimationSystem>();
		m_inputSystem = std::make_unique<System::InputSystem>();
		m_cameraSystem = std::make_unique<System::CameraSystem>();


		Components::InputAction actionSpace("Test");
		Components::InputAction actionMouse("Test_Mouse");
		Components::InputAction actionMouseMotion("Test_Mouse_Motion");
		actionSpace.AddBinding(SDL_SCANCODE_SPACE);
		actionMouse.AddMouseButtonBinding(SDL_BUTTON_LEFT);
		actionMouseMotion.AddMouseMotionBinding();

		m_inputSystem->RegisterAction(actionSpace);
		m_inputSystem->RegisterAction(actionMouse);
		m_inputSystem->RegisterAction(actionMouseMotion);
		Components::InputBinding inputActionSpace = Components::InputBinding::Keyboard(SDL_SCANCODE_SPACE);
		Components::InputBinding inputActionMouseLeft = Components::InputBinding::MouseButton(SDL_BUTTON_LEFT);
		Components::InputBinding inputActionMouseMotion = Components::InputBinding::MouseMotion();

		Components::Camera camera;
		m_cameraSystem->AddCamera("Main", camera);
		m_cameraSystem->SetCurrentCamera("Main");

	}


	void ScenePlay::Update(const float deltaTime)
	{
		if (m_inputSystem->IsActionPressed("Test_Mouse")) {
			Vec2 mouseDelta = m_inputSystem->GetMouseDelta("Test_Mouse_Motion");
			m_cameraSystem->AdjustCameraPosition(mouseDelta);
		}


	}
	void ScenePlay::HandleInput(SDL_Event& event)
	{
		m_inputSystem->HandleInput(event);
	}
	void ScenePlay::Render(SDL_Renderer& renderer)
	{
		m_renderSystem->RenderTileMap(m_tilemap, renderer, *m_cameraSystem);
		m_renderSystem->Render(m_Registry, renderer);
	}
}