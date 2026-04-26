#pragma once
#include <string>
#include <SDL3/SDL.h>

namespace Components
{
	// Text alignment options
	enum class TextAlign
	{
		Left,
		Center,
		Right
	};

	// ECS component — attach to any entity with a Transform2D to render text
	struct Text
	{
		std::string text;                               // The string to display
		std::string fontPath;                           // Path to the .ttf font file
		float       fontSize    = 24.0f;                // Font size in points
		Color   color       = { 255, 255, 255, 255 }; // Text color (default white)
		TextAlign   align       = TextAlign::Left;
		bool        visible     = true;
		int         layer       = 0;                    // Render layer (same as Sprite layers)
		bool        wordWrap    = false;                // Enable word wrapping
		int         wrapWidth   = 0;                    // Max width in pixels before wrapping (0 = no limit)

		// Internal — managed by the renderer. Do NOT set manually.
		uint32_t    _cachedFontId   = 0;    // Cached font lookup ID
		std::string _cachedText;            // Last rendered text (dirty check)
		float       _cachedFontSize = 0.0f; // Last rendered size (dirty check)
		Color   _cachedColor    = { 0, 0, 0, 0 };
		uint32_t    _cachedTextureId = 0;   // Cached texture ID in renderer
		float       _cachedWidth    = 0.0f; // Width of last rendered text texture
		float       _cachedHeight   = 0.0f; // Height of last rendered text texture

		Text() = default;
		Text(const std::string& text, const std::string& fontPath, float fontSize = 24.0f)
			: text(text), fontPath(fontPath), fontSize(fontSize) {}
	};
}
