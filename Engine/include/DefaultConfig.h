#pragma once

#include <string>
#include <SDL3/SDL_video.h>

class WindowConfig {
	public:
	const char* title = "Default Title";
	int width = 640;
	int height = 360;
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