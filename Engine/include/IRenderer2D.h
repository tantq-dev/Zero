#pragma once
#include "Components.h"
#include <cstdint>
#include <CoreComponents.h>

class IRenderer2D {
public:
	virtual ~IRenderer2D() = default;

	// Sprite rendering
	virtual void PushSpriteToRenderQueue(const Components::Sprite&, const Components::Transform2D&) = 0;
	virtual Components::Texture GetTextureFromFile(const std::string& path) = 0;
	virtual void CallRender() = 0;
	virtual void SetCamera(Components::CameraComponent* cam) = 0;
	virtual Components::CameraBounds GetCameraBounds() const = 0;

	virtual Vec2 ScreenToWorld(Vec2 screenPos) = 0;

	// Text rendering
	virtual uint32_t LoadFont(const std::string& path, float size) = 0;
	virtual void UnloadFont(uint32_t fontId) = 0;

	/// Render text string to an internal texture. Returns a texture ID.
	/// If the text/font/size/color hasn't changed, returns the cached texture.
	virtual uint32_t RenderTextToTexture(uint32_t fontId,
		const std::string& text,
		Components::Color color,
		bool wordWrap, int wrapWidth,
		float& outWidth, float& outHeight) = 0;

	/// Push a text texture to the render queue at the given transform and layer.
	virtual void PushTextToRenderQueue(uint32_t textureId,
		float width, float height,
		const Components::Transform2D& transform,
		int layer,
		Components::TextAlign align) = 0;

	// Primitive rendering
	virtual void DrawRect(Components::Rect rect, Components::Color color, bool fill, int layer) = 0;
	virtual void DrawLine(float x1, float y1, float x2, float y2, Components::Color color, int layer) = 0;

	virtual void BeginFrame() = 0;
	virtual void EndFrame(int windowW, int windowH) = 0;
};