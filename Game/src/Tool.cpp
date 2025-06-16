#include "Tool.h"
#include "config/ApplicationConfig.h"
#include <random>
#include "resources/ResourcesManager.h"
#include "utilities/Logger.h"
#include "core/EventSystem.h"
#include "MonsterModel.h"
#include "EventKey.h"


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

namespace Tool
{
	void Tool::Initialize(SDL_Renderer& renderer)
	{
		//todo: clean up Magic number and magic string when function work correcly


		ResourcesManager::GetInstance().LoadTextures(MonsterTextureMap, &renderer);

		m_physicSystem = std::make_unique<System::PhysicSystem>();
		m_renderSystem = std::make_unique<System::RenderSystem>();
		m_animationSystem = std::make_unique<System::AnimationSystem>();
		m_inputSystem = std::make_unique<System::InputSystem>();
		m_cameraSystem = std::make_unique<System::CameraSystem>();
		m_gridSystem = std::make_unique<System::GridSystem>(Components::Grid(10, 10, 24, 12));
		m_uiManager = std::make_unique<UI::UIManager>();
		m_mapEditor = std::make_unique<MapEditor>();
		m_dataHandler = std::make_unique<DataHandler>();


		//m_uiManager->Initialize();

		Components::InputAction actionSpace("Test");
		Components::InputAction actionMouse("Test_Mouse");
		Components::InputAction actionMouseMotion("Test_Mouse_Motion");
		Components::InputAction actionMouseWheel("Test_Mouse_Wheel");
		Components::InputAction actionMiddleMouse("Test_Mouse_Middle");
		Components::InputAction actionRightMouse("Right_Mouse");

		actionSpace.AddBinding(SDL_SCANCODE_SPACE);
		actionMouse.AddMouseButtonBinding(SDL_BUTTON_LEFT);
		actionMouseMotion.AddMouseMotionBinding();
		actionMouseWheel.AddMouseWheelBinding();
		actionMiddleMouse.AddMouseButtonBinding(SDL_BUTTON_MIDDLE);
		actionRightMouse.AddMouseButtonBinding(SDL_BUTTON_RIGHT);

		m_inputSystem->RegisterAction(actionSpace);
		m_inputSystem->RegisterAction(actionMouse);
		m_inputSystem->RegisterAction(actionMouseMotion);
		m_inputSystem->RegisterAction(actionMouseWheel);
		m_inputSystem->RegisterAction(actionMiddleMouse);
		m_inputSystem->RegisterAction(actionRightMouse);


		Components::Camera camera;
		camera.SetZoom(6); // default zoom
		m_cameraSystem->AddCamera("Main", camera);
		m_cameraSystem->SetCurrentCamera("Main");

		Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromPalette,
			[this](const Core::EventData& data) {
				MonsterItem selectedMonster = data.get<MonsterTypeDefinition>().item;
				LOG_INFO("Monster selected in palate: " + selectedMonster.name);
			});
		Core::EventSystem::getInstance().subscribe(EventKeys::ExportBullet,
			[this](const Core::EventData& data) {
				// Access the registry pointer from the event data
				auto* registry = data.get<MonsterTypeRegistry*>();
				if (registry) {
					auto monsters = registry->GetAllMonsterTypes();
					m_dataHandler->ExportBulletConfig(monsters);
				}
			});
	}


	void Tool::Update(const float deltaTime)
	{
		//reset grid
		m_gridSystem->ResetGridHightlight();

		if (m_inputSystem->IsActionHeld("Test_Mouse_Middle")) {
			Vec2 mouseDelta = m_inputSystem->GetMousePosition("Test_Mouse_Motion") - m_lastMousePosition;
			m_lastMousePosition = m_inputSystem->GetMousePosition("Test_Mouse_Motion");

			if (mouseDelta == m_inputSystem->GetMousePosition("Test_Mouse_Motion")) {
				return; // Skip the first frame to avoid a large jump
			}
			m_cameraSystem->AdjustCameraPosition(mouseDelta);
		}
		else {
			m_lastMousePosition = Vec2::zero();
		}


		//hight light cell
		Vec2 mousePos = m_inputSystem->GetMousePosition("Test_Mouse_Motion");
		Vec2 mouseWorld = { 0,0 };
		m_cameraSystem->CameraViewToWorld(mousePos, mouseWorld);
		if (mouseWorld.x > m_gridSystem->GetGridWidth() / 2 ||
			mouseWorld.x < -m_gridSystem->GetGridWidth() / 2 ||
			mouseWorld.y > m_gridSystem->GetGridHeight() / 2 ||
			mouseWorld.y < -m_gridSystem->GetGridHeight() / 2) {
			//out of grid bound
		}
		else {
			auto pCell = m_gridSystem->GetCell(mouseWorld);

			if (pCell) {
				pCell->isColor = !pCell->isColor;
				if (m_inputSystem->IsActionPressed("Test_Mouse"))
				{
					//todo: place monster at this grid
					m_mapEditor->ClickOnMap(pCell->GetCenter());
				}
				else if (m_inputSystem->IsActionPressed("Right_Mouse"))
				{
					m_mapEditor->DeleteFromMap(pCell->GetCenter());
				}
			}
		}


		m_cameraSystem->AdjustCameraZoom(m_inputSystem->GetMouseWheelDelta("Test_Mouse_Wheel"));
		m_inputSystem->ResetMouseWheelDelta("Test_Mouse_Wheel");
		m_inputSystem->ResetMousePress("Test_Mouse");
		m_inputSystem->ResetMousePress("Test_Mouse_Middle");
		m_inputSystem->ResetMousePress("Right_Mouse");

	}
	void Tool::HandleInput(SDL_Event& event)
	{
		m_inputSystem->HandleInput(event);
	}
	void Tool::HandleUI(SDL_Event& event)
	{
		m_uiManager->Render();
	}
	void Tool::Render(SDL_Renderer& renderer)
	{
		m_renderSystem->RenderSprite(m_mapEditor->GetMonsterRegistry(), renderer, *m_cameraSystem);
		m_renderSystem->RenderGrid(m_gridSystem->GetGrid(), renderer, *m_cameraSystem);
		m_renderSystem->RenderAnimation(m_Registry, renderer);
	}
}