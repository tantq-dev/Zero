#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
#include <vector>
#include <string>
#include <unordered_map>
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
		

		Sprite() = default;
		explicit Sprite(Texture texture)
			: texture(texture)
		{
		}
	};
	



	struct SpriteAtlasAnimation {
		bool loop = true;
		std::vector<size_t> frames; // Indices into the atlas frames array
		float frameDuration = 0.1f;
	};

	struct SpriteAtlas {
		Texture texture = {};
		std::vector<Rect> frames;
		std::unordered_map<std::string, SpriteAtlasAnimation> animations;
	};



	struct Animation {
		uint32_t atlasId = 0; // ID from ResourcesManager
		std::string currentAnimationName;
		size_t currentFrameIndex = 0; // Index into SpriteAtlasAnimation::frames
		float currentFrameTime = 0;
		bool isFinished = false;
		bool isPlaying = true;

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
