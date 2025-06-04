#include "ScenePlay.h"
#include <ApplicationConfig.h>
#include <random>
#include "ResourcesManager.h"
#include "Logger.h"


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
	
		ResourcesManager::GetInstance().LoadTexture("Link", "assets/textures/link.bmp", &renderer);
		ResourcesManager::GetInstance().LoadTexture("Pirate_Run", "assets/textures/pirate_run.bmp", &renderer);
		ResourcesManager::GetInstance().LoadTexture("Pirate", "assets/textures/pirate.bmp", &renderer);



		m_physicSystem = std::make_unique<System::PhysicSystem>();
		m_renderSystem = std::make_unique<System::RenderSystem>();
		m_animationSystem = std::make_unique<System::AnimationSystem>();
		m_inputSystem = std::make_unique<System::InputSystem>();


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
		// Create 10 entities with CTransform component at random positions
		for (size_t i = 0; i < 5; i++)
		{
			entt::entity e = m_Registry.create();
			float randomX = RandomFloat(0.0f, ApplicationConfig::DEFAULT_WINDOW_WIDTH);
			float randomY = RandomFloat(0.0f, ApplicationConfig::DEFAULT_WINDOW_HEIGHT);

			float randomVelocityX = RandomFloat(-10.0f, 10.0f);
			float randomVelocityY = RandomFloat(-10.0f, 10.0f);

			float randomScaleX = 128.0f;
			float randomScaleY = 80.0f;

			m_Registry.emplace<Components::Transform>(e, Vec2(randomX, randomY), Vec2(randomScaleX, randomScaleY), 10.0f);
			m_Registry.emplace<Components::Velocity>(e, Vec2(randomVelocityX, randomVelocityY));
			m_Registry.emplace<Components::Collider>(e, Components::Collider::MakeBox(Vec2(randomScaleX, randomScaleX)));
			m_Registry.emplace<Components::Sprite>(e, ResourcesManager::GetInstance().GetTexture("Pirate"));

			auto texture = ResourcesManager::GetInstance().GetTexture("Pirate_Run");
			auto texturePirate = ResourcesManager::GetInstance().GetTexture("Pirate");
			auto animation = new Components::Animation(texture,texture->w/6,texture->h,0.1f,6);
			auto animation2 = new Components::Animation(texturePirate, texturePirate->w / 5, texturePirate->h, 0.1f, 5);
			m_Registry.emplace<Components::Animator>(e);

			auto& animator = m_Registry.get<Components::Animator>(e);
			animator.AddAnimation("Run", *animation);
			animator.AddAnimation("Idle", *animation2);
			animator.SetCurrentAnimation("Run");
		}
	}


	void ScenePlay::Update(const float deltaTime)
	{
		m_physicSystem->Update(deltaTime, m_Registry);
		auto group = m_Registry.group(entt::get<Components::Transform, Components::Velocity,Components::Collider>);

		if (m_inputSystem->IsActionPressed("Test"))
		{
			LOG_INFO("Test button pressed");
		}

		Vec2 mousePosition = m_inputSystem->GetMousePosition("Test_Mouse_Motion");

		for (auto& entity : group)
		{
			auto& transform = group.get<Components::Transform>(entity);
			auto& velocity = group.get<Components::Velocity>(entity);
			auto direction = mousePosition - transform.position;
			const auto& collider = group.get<Components::Collider>(entity);
			
			transform.position.x += direction.x * deltaTime;
			transform.position.y += direction.y * deltaTime;
		}
		m_animationSystem->Update(m_Registry,deltaTime);

	}
	void ScenePlay::HandleInput(SDL_Event &event)
	{
		m_inputSystem->HandleInput(event);
	}
	void ScenePlay::Render(SDL_Renderer& renderer)
	{
		m_renderSystem->Render( m_Registry, renderer);
	}
}