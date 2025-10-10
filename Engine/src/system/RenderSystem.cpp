#include "RenderSystem.h"
namespace System
{
	
	void RenderSystem::Update(entt::registry& registry)
	{
		auto spriteView = registry.group<Components::Sprite, Components::Transform2D>();
		for (auto entity : spriteView)
		{
			auto& sprite = spriteView.get<Components::Sprite>(entity);
			auto& transform = spriteView.get<Components::Transform2D>(entity);
			if (m_renderer) {
				m_renderer->PushSpriteToRenderQueue(sprite, transform);
			}
		}
		if (m_renderer) {
			m_renderer->CallRender();
		}
	}

}

