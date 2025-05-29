#include "ScenePlay.h"
#include <ApplicationConfig.h>
#include <random>

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
	
		ResourcesManager::GetInstance().LoadTexture("Link", "assets/link.bmp", &renderer);

		ResourcesManager::GetInstance().LoadTexture("Pirate", "assets/pirate.bmp", &renderer);
		m_physicSystem = std::make_unique<System::PhysicSystem>();
		m_renderSystem = std::make_unique<System::RenderSystem>();
		m_animationSystem = std::make_unique<System::AnimationSystem>();
		// Create 10 entities with CTransform component

		entt::entity e = m_Registry.create();



		// Create 10 entities with CTransform component at random positions
		for (size_t i = 0; i < 5; i++)
		{
			entt::entity e = m_Registry.create();
			float randomX = RandomFloat(0.0f, ApplicationConfig::DEFAULT_WINDOW_WIDTH);
			float randomY = RandomFloat(0.0f, ApplicationConfig::DEFAULT_WINDOW_HEIGHT);

			float randomVelocityX = RandomFloat(-10.0f, 10.0f);
			float randomVelocityY = RandomFloat(-10.0f, 10.0f);

			float randomScaleX = 40.0f;
			float randomScaleY = 40.0f;

			m_Registry.emplace<Components::Transform>(e, Vec2(randomX, randomY), Vec2(randomScaleX, randomScaleY), 10.0f);
			m_Registry.emplace<Components::Velocity>(e, Vec2(randomVelocityX, randomVelocityY));
			m_Registry.emplace<Components::Collider>(e, Components::Collider::MakeBox(Vec2(randomScaleX, randomScaleX)));
			m_Registry.emplace<Components::Sprite>(e, ResourcesManager::GetInstance().GetTexture("Pirate"));
			m_Registry.emplace<Components::Animation>(e,64,2);
		}
	}


	void ScenePlay::Update(float deltaTime)
	{
		m_physicSystem->Update(deltaTime, m_Registry);

		auto group = m_Registry.group(entt::get<Components::Transform, Components::Velocity,Components::Collider>);

		for (auto& entity : group)
		{
			auto& transform = group.get<Components::Transform>(entity);
			auto& velocity = group.get<Components::Velocity>(entity);
			auto& collider = group.get<Components::Collider>(entity);
			if (collider.isColliding)
			{
				velocity.velocity.x = -velocity.velocity.x;
				velocity.velocity.y = -velocity.velocity.y;
			}
			transform.position.x += velocity.velocity.x * deltaTime * 10;
			transform.position.y += velocity.velocity.y * deltaTime * 10;
		}
		m_animationSystem->Update(m_Registry,deltaTime);

	}
	void ScenePlay::SDoAction()
	{

	}
	void ScenePlay::Render(SDL_Renderer& renderer)
	{
		m_renderSystem->Render( m_Registry, renderer);
	}
}