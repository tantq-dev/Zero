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

namespace Core
{
	void ScenePlay::Initialize()
	{
		physicSystem = std::make_unique<System::PhysicSystem>();
		// Create 10 entities with CTransform component

		entt::entity e = m_Registry.create();

		physicSystem = std::make_unique<System::PhysicSystem>();

		// Create 10 entities with CTransform component at random positions
		for (size_t i = 0; i < 50; i++)
		{
			entt::entity e = m_Registry.create();
			float randomX = RandomFloat(0.0f, ApplicationConfig::DEFAULT_WINDOW_WIDTH);
			float randomY = RandomFloat(0.0f, ApplicationConfig::DEFAULT_WINDOW_HEIGHT);

			float randomVelocityX = RandomFloat(-3.0f, 3.0f);
			float randomVelocityY = RandomFloat(-3.0f, 3.0f);

			float randomScaleX = RandomFloat(5.0f, 15.0f);
			float randomScaleY = RandomFloat(5.0f, 15.0f);

			m_Registry.emplace<Components::Transform>(e, Vec2(randomX, randomY), Vec2(randomScaleX, randomScaleY), 10.0f);
			m_Registry.emplace<Components::Velocity>(e, Vec2(randomVelocityX, randomVelocityY));
			m_Registry.emplace<Components::BoxCollider>(e, Vec2(randomScaleX, randomScaleX));
		}
	}


	void ScenePlay::Update(float deltaTime)
	{
		physicSystem->Update(deltaTime, m_Registry);
	}
	void ScenePlay::SRender(SDL_Renderer* render)
	{
		auto view = m_Registry.view<Components::Transform>();

		for (auto entity : view)
		{
			auto& transform = view.get<Components::Transform>(entity);

			SDL_FRect rect;
			rect.x = static_cast<int>(transform.position.x);
			rect.y = static_cast<int>(transform.position.y);
			rect.w = static_cast<int>(transform.scale.x);
			rect.h = static_cast<int>(transform.scale.y);

			SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
			SDL_RenderFillRect(render, &rect);
		}

	}
	void ScenePlay::SDoAction()
	{

	}
}