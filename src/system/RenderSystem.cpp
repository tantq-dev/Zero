#include "RenderSystem.h"
namespace System
{
	void RenderSystem::Render(entt::registry& registry, SDL_Renderer& renderer)
	{
		auto view = registry.group<>(entt::get<Components::Transform, Components::Animator>);
	
		SDL_FRect* srcRect = NULL;
		for (auto entity : view)
		{
			auto& transform = view.get<Components::Transform>(entity);

			if (registry.all_of<Components::Animator>(entity))
			{
				auto& animator = registry.get<Components::Animator>(entity);
				auto animation = animator.GetCurrentAnimation();
				srcRect = new SDL_FRect{
					animation->currentFrame * animation->frameWidth,
					0,
					animation->frameWidth,
					animation->frameHeight

				};
				m_dstRect.x = static_cast<int>(transform.position.x - transform.scale.x / 2);
				m_dstRect.y = static_cast<int>(transform.position.y - transform.scale.y / 2);
				m_dstRect.w = static_cast<int>(transform.scale.x);
				m_dstRect.h = static_cast<int>(transform.scale.y);

				SDL_RenderTexture(&renderer, animation->texture , srcRect, &m_dstRect);
			}


			
		}
		SDL_RenderPresent(&renderer);
	}

}

