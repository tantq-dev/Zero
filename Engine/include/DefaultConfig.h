#pragma once

#include <string>
#include <SDL3/SDL_video.h>

class WindowConfig {
	public:
	const char* title = "Default Title";
	int width = 1280;
	int height = 720;
#ifdef __EMSCRIPTEN__
	Uint32 flags = SDL_WINDOW_RESIZABLE;
#else
	Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
#endif
	WindowConfig() = default;
	WindowConfig(const char* t, int w, int h, Uint32 f)
		: title(t), width(w), height(h), flags(f) {
	}
};

namespace EngieResources {
	constexpr std::string_view ENGINE_ASSETS_PATH = "engine_assets";
	constexpr std::string_view DEFAULT_FONT = "engine_assets/fonts/default.ttf";
}