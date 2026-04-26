#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
#include <vector>
#include <string>
#include <cmath>

namespace Components
{
	struct Size
	{
		float width = 0.0f;
		float height = 0.0f;
		Size() = default;
		Size(float w, float h)
			: width(w), height(h)
		{
		}
	};

	struct Rect {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
		Rect() = default;
		Rect(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h)
		{
		}
	};

	struct Texture
	{
		uint32_t id = 0;
		Size size = { 0.0f, 0.0f };
		Texture() = default;
		Texture(uint32_t texID, const Size& sz)
			: id(texID), size(sz)
		{
		}
	};

	struct Sprite
	{
		Texture texture = {};
		bool flipHorizontal = false;
		bool visible = true;
		int layer = 0;
		float opacity = 1;
		Vec2 pivot = { 0.5f, 0.5f }; // Normalized (0 to 1)
		Rect source = { 0, 0, 0, 0 }; //
		SDL_Color tint = { 255, 255, 255, 255 }; // Default white (no tint)
		
		// Spritesheet support
		uint32_t spriteSheetId = 0;
		size_t frameIndex = 0;

		Sprite() = default;
		explicit Sprite(Texture texture)
			: texture(texture)
		{
		}
	};
	

	// Optional: asset describing a sprite sheet (kept in a DB, not a component)
	struct SpriteSheet {
		Texture texture = {};
		std::vector<Rect> frames;      // rectangles per frame
	};

	struct AnimationClip {
		uint32_t spriteSheetId = 0;
		size_t frameIndexStart = 0;
		size_t numberOfFrames = 0;
		bool isLoop = true;
		float frameTime = 0;
	};

	struct Animation {
		AnimationClip currentClip = {};
		size_t currentFrame = 0;
		float currentFrameTime = 0;
		bool isFinished = false;
	};

	struct Color {
		float r;
		float g;
		float b;
		float a;
	};

	struct Shape {
		enum class Type { Rect, Circle } type = Type::Rect;
		Color color = { 255, 255, 255, 255 };
		bool fill = true;
		int layer = 0;
		float radius = 0.0f; // for Circle
		Size size = { 0, 0 }; // for Rect
		bool visible = true;
	};


	struct CameraBounds {
		float x, y;
		float width, height;
	};
}
