#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <functional>
#include "DefaultConfig.h"

namespace Core
{
	class Window
	{
	private:
		std::shared_ptr<SDL_Window> m_window = nullptr;
		std::shared_ptr<SDL_Renderer> m_renderer = nullptr;

	public:
		Window() = default;
		Window(const char* title, int width, int height, Uint32 flags);
		Window(const WindowConfig&);
		~Window() = default;

		void Initialize(const char* title, int width, int height, Uint32 flags = NULL);
		void Initialize(const WindowConfig& config) {
			Initialize(config.title, config.width, config.height, config.flags);
		}
		std::shared_ptr<SDL_Window> GetWindow() const { return m_window; }
		std::shared_ptr<SDL_Renderer> GetRenderer() const { return m_renderer; }
		void Resize(int width, int height);
		void Clear(uint8_t r = 30, uint8_t g = 30, uint8_t b = 30, uint8_t a = 255);
		void Present();
		void Close();

		bool ProcessEvents(std::function<void(SDL_Event&)> eventCallback);
		uint64_t GetPerformanceCounter() const;
		uint64_t GetPerformanceFrequency() const;
		void Delay(uint32_t ms) const;
	};
}