#include "ScenePlay.h"

namespace Core
{
	void ScenePlay::Initialize()
	{
		// Create 10 entities with CTransform component
		for (size_t i = 0; i < 10; i++)
		{
			entt::entity e = m_Registry.create();
			m_Registry.emplace<CTransform>(e, Vec2(i *5.0f * 3 + i, i * 4.0f * 2 + i), Vec2(20.0f, 20.0f),10.0f);
			m_Registry.emplace<CPhysic>(e, Vec2((float)((i + 1) *10 + i), (float)((i+1)*10 + i)));
		}

	}
	void ScenePlay::Update(float deltaTime)
	{
		auto group = m_Registry.group<CTransform,CPhysic>();
		for (auto entity: group)
		{
			auto& transform = group.get<CTransform>(entity);
			auto& physic = group.get<CPhysic>(entity);
			transform.position.x += physic.velocity.x * deltaTime;
			transform.position.y += physic.velocity.y * deltaTime;
			
			if (transform.position.x > ApplicationConfig::DEFAULT_WINDOW_WIDTH && physic.velocity.x > 0 || transform.position.x < 0 && physic.velocity.x < 0) 
			{
				physic.velocity.x = -physic.velocity.x; // Reverse direction
			}

			if (transform.position.y > ApplicationConfig::DEFAULT_WINDOW_HEIGHT && physic.velocity.y > 0 || transform.position.y < 0 && physic.velocity.y < 0) 
			{
				physic.velocity.y = -physic.velocity.y; // Reverse direction
			}
		
		}
	}
	void ScenePlay::SRender(SDL_Renderer* render)
	{
		auto view = m_Registry.view<CTransform>();

		for (auto entity : view)
		{
			auto& transform = view.get<CTransform>(entity);
	
            SDL_FRect rect;
            rect.x = static_cast<int>(transform.position.x);
            rect.y = static_cast<int>(transform.position.y);
            rect.w = static_cast<int>(transform.scale.x);
            rect.h = static_cast<int>(transform.scale.y);

            SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
			SDL_RenderFillRect(render,&rect);
		}
		
	}
	void ScenePlay::SDoAction()
	{

	}
}