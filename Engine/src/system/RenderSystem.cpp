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
		
		// Handle text rendering
		auto textView = registry.view<Components::Text, Components::Transform2D>();
		for (auto entity : textView)
		{
			auto& text = textView.get<Components::Text>(entity);
			auto& transform = textView.get<Components::Transform2D>(entity);

			if (!text.visible) continue;

			// Load font if needed
			if (text._cachedFontId == 0 && !text.fontPath.empty())
			{
				text._cachedFontId = m_renderer->LoadFont(text.fontPath, text.fontSize);
			}

			if (text._cachedFontId == 0) continue;

			// Dirty check for texture regeneration
			bool isDirty = (text.text != text._cachedText) ||
						   (text.fontSize != text._cachedFontSize) ||
						   (text.color.r != text._cachedColor.r || text.color.g != text._cachedColor.g || text.color.b != text._cachedColor.b || text.color.a != text._cachedColor.a);

			if (isDirty || text._cachedTextureId == 0)
			{
				float w, h;
				text._cachedTextureId = m_renderer->RenderTextToTexture(
					text._cachedFontId, text.text, text.color,
					text.wordWrap, text.wrapWidth, w, h
				);
				text._cachedText = text.text;
				text._cachedFontSize = text.fontSize;
				text._cachedColor = text.color;
				text._cachedWidth = w;
				text._cachedHeight = h;
			}

			if (text._cachedTextureId != 0)
			{
				m_renderer->PushTextToRenderQueue(
					text._cachedTextureId, text._cachedWidth, text._cachedHeight,
					transform, text.layer, text.align
				);
			}
		}
		
		m_renderer->CallRender();
	}

}

