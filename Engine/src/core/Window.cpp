#include "Window.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>

namespace Core
{
	Window::Window(const char* title, int width, int height, Uint32 flags)
	{
		Initialize(title, width, height, flags);
	}

	Window::Window(const WindowConfig& config)
	{
		Initialize(config);
	}

	void Window::Initialize(const char* title, int width, int height, Uint32 flags)
	{
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		{
			throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
		}

		if (!TTF_Init())
		{
			throw std::runtime_error(std::string("TTF_Init Error: ") + SDL_GetError());
		}

		auto window = SDL_CreateWindow(title, width, height, flags);
		if (!window)
		{
			std::string error = SDL_GetError();
			throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + error);
		}
		m_window = std::shared_ptr<SDL_Window>(window, SDL_DestroyWindow);

		auto render = SDL_CreateRenderer(m_window.get(), nullptr);
		if (!render)
		{
			std::string error = SDL_GetError();
			throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + error);
		}
		m_renderer = std::shared_ptr<SDL_Renderer>(render, SDL_DestroyRenderer);
	}

	void Window::Resize(int width, int height)
	{

	}

	void Window::Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
		SDL_RenderClear(m_renderer.get());
	}

	void Window::Present()
	{
		SDL_RenderPresent(m_renderer.get());
	}

	void Window::Close()
	{
		m_window.reset();
		m_renderer.reset();
		TTF_Quit();
		SDL_Quit();
	}

	bool Window::ProcessEvents(std::function<void(SDL_Event&)> eventCallback)
	{
		SDL_Event event;
		bool running = true;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
			if (eventCallback)
			{
				eventCallback(event);
			}
		}
		return running;
	}

	uint64_t Window::GetPerformanceCounter() const
	{
		return SDL_GetPerformanceCounter();
	}

	uint64_t Window::GetPerformanceFrequency() const
	{
		return SDL_GetPerformanceFrequency();
	}

	void Window::Delay(uint32_t ms) const
	{
		SDL_Delay(ms);
	}

}