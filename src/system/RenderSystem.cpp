#include "RenderSystem.h"
namespace System
{
	void RenderSystem::Render(entt::registry& registry, SDL_Renderer& renderer)
	{
		auto view = registry.group<>(entt::get<Components::Transform, Components::Sprite>);
	
		for (auto entity : view)
		{
			auto& transform = view.get<Components::Transform>(entity);
			auto& sprite = view.get<Components::Sprite>(entity);

			m_dstRect.x = static_cast<int>(transform.position.x - transform.scale.x/2);
			m_dstRect.y = static_cast<int>(transform.position.y - transform.scale.y/2);
			m_dstRect.w = static_cast<int>(transform.scale.x);
			m_dstRect.h = static_cast<int>(transform.scale.y);

			SDL_RenderTexture(&renderer, sprite.texture, NULL, &m_dstRect);
		}
		SDL_RenderPresent(&renderer);
	}

}

