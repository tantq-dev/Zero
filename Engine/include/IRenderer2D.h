#pragma once
#include "Components.h"
#include <cstdint>

class IRenderer2D {
public:
	virtual ~IRenderer2D() = default;

	// Sprite rendering
	virtual void PushSpriteToRenderQueue(const Components::Sprite&, const Components::Transform2D&) = 0;
	virtual Components::Texture GetTextureFromFile(const std::string& path) = 0;
	virtual void CallRender() = 0;

	// Text rendering
	virtual uint32_t LoadFont(const std::string& path, float size) = 0;
	virtual void UnloadFont(uint32_t fontId) = 0;

	/// Render text string to an internal texture. Returns a texture ID.
	/// If the text/font/size/color hasn't changed, returns the cached texture.
	virtual uint32_t RenderTextToTexture(uint32_t fontId,
		const std::string& text,
		SDL_Color color,
		bool wordWrap, int wrapWidth,
		float& outWidth, float& outHeight) = 0;

	/// Push a text texture to the render queue at the given transform and layer.
	virtual void PushTextToRenderQueue(uint32_t textureId,
		float width, float height,
		const Components::Transform2D& transform,
		int layer,
		Components::TextAlign align) = 0;
};