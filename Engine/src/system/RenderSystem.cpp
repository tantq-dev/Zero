#include "RenderSystem.h"
namespace System
{
	static bool IsVisible(float x, float y, float w, float h,
		const Components::CameraBounds& cam)
	{
		float left = x - w * 0.5f;
		float right = x + w * 0.5f;
		float top = y - h * 0.5f;
		float bottom = y + h * 0.5f;

		return !(right < cam.x || left > cam.x + cam.width ||
			bottom < cam.y || top > cam.y + cam.height);
	}
	
	void RenderSystem::Update(entt::registry& registry)
	{
		if (!m_renderer) return;

		// Auto-detect and set camera from registry
		auto camView = registry.view<Components::CameraComponent>();
		if (!camView.empty())
		{
			auto& camComp = registry.get<Components::CameraComponent>(camView.front());
			m_renderer->SetCamera(&camComp);
		}
		else
		{
			m_renderer->SetCamera(nullptr);
		}

		auto cam = m_renderer->GetCameraBounds();
		// SPRITES
		// =======================
		auto spriteView = registry.group<Components::Sprite, Components::Transform2D>();
		for (auto entity : spriteView)
		{
			auto [sprite, transform] = spriteView.get<Components::Sprite, Components::Transform2D>(entity);

			float width = 0.f;
			float height = 0.f;

			Components::Sprite finalSprite = sprite;

			width = sprite.source.w * transform.scale.x;
			height = sprite.source.h * transform.scale.y;
			
			if (!IsVisible(transform.position.x, transform.position.y, width, height, cam))
				continue;

			m_renderer->PushSpriteToRenderQueue(finalSprite, transform);
		}

		// =======================
		// TEXT
		// =======================
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

			// Dirty check
			bool isDirty =
				(text.text != text._cachedText) ||
				(text.fontSize != text._cachedFontSize) ||
				(text.color.r != text._cachedColor.r ||
					text.color.g != text._cachedColor.g ||
					text.color.b != text._cachedColor.b ||
					text.color.a != text._cachedColor.a);

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

			if (text._cachedTextureId == 0) continue;

			// Culling
			if (!IsVisible(transform.position.x, transform.position.y,
				text._cachedWidth, text._cachedHeight, cam))
				continue;

			m_renderer->PushTextToRenderQueue(
				text._cachedTextureId,
				text._cachedWidth,
				text._cachedHeight,
				transform,
				text.layer,
				text.align,
				transform.position.y
			);
		}

		// =======================
		// SHAPES
		// =======================
		auto shapeView = registry.view<Components::Shape, Components::Transform2D>();
		for (auto entity : shapeView)
		{
			auto& shape = shapeView.get<Components::Shape>(entity);
			auto& transform = shapeView.get<Components::Transform2D>(entity);

			if (!shape.visible) continue;

			if (shape.type == Components::Shape::Type::Rect)
			{
				float width = shape.size.width * transform.scale.x;
				float height = shape.size.height * transform.scale.y;

				if (!IsVisible(transform.position.x, transform.position.y, width, height, cam))
					continue;

				Components::Rect rect{
					transform.position.x - width * 0.5f,
					transform.position.y - height * 0.5f,
					width,
					height
				};

				m_renderer->DrawRect(rect, shape.color, shape.fill, shape.layer, transform.position.y);
			}
			else if (shape.type == Components::Shape::Type::Circle)
			{
				float size = shape.radius * 2.0f;
				if (!IsVisible(transform.position.x, transform.position.y, size, size, cam))
					continue;

				Components::Rect rect{
					transform.position.x - shape.radius,
					transform.position.y - shape.radius,
					size,
					size
				};
				m_renderer->DrawRect(rect, shape.color, shape.fill, shape.layer, transform.position.y);
			}
		}
	}
}

