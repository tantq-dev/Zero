#include "RenderSystem.h"
namespace System
{
	
	void RenderSystem::Update(entt::registry& registry)
	{
		if (!m_renderer) { return; }
		
		auto spriteView = registry.group<Components::Sprite, Components::Transform2D>();
		for (auto entity : spriteView)
		{
			auto [sprite, transform] = spriteView.get<Components::Sprite, Components::Transform2D>(entity);
			
			// Handle spritesheet rendering
			if (sprite.spriteSheetId != 0 && m_resources)
			{
				// Get spritesheet from resources
				const auto* spriteSheet = m_resources->GetSpriteSheet(sprite.spriteSheetId);
				if (spriteSheet && sprite.frameIndex < spriteSheet->frames.size())
				{
					// Create a temporary sprite with the correct source rect from spritesheet
					Components::Sprite frameSprite = sprite;
					frameSprite.texture = spriteSheet->texture;
					frameSprite.source = spriteSheet->frames[sprite.frameIndex];
					
					m_renderer->PushSpriteToRenderQueue(frameSprite, transform);
				}
			}
			else
			{
				// Regular sprite rendering
				m_renderer->PushSpriteToRenderQueue(sprite, transform);
			}
		}
		
		m_renderer->CallRender();
	}

}

