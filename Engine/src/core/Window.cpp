#include "Window.h"
#include <stdexcept>

namespace Core
{
    Window::Window(const char* title, int width, int height, Uint32 flags)
    {
        Initialize(title, width, height, flags);
    }

    void Window::Initialize(const char* title, int width, int height, Uint32 flags)
    {
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

    void Window::Clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
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
		SDL_DestroyWindow(m_window.get());
	}

}