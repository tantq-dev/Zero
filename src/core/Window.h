#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <string>

namespace Core
{
    class Window
    {
    private:
        std::shared_ptr<SDL_Window> m_window = nullptr;
        std::shared_ptr<SDL_Renderer> m_renderer = nullptr;

    public:
        Window() = default;
        Window(const char* title, int width, int height, Uint32 flags = SDL_WINDOW_RESIZABLE);
        ~Window() = default;

        void Initialize(const char* title, int width, int height, Uint32 flags = SDL_WINDOW_RESIZABLE);
        
        std::shared_ptr<SDL_Window> GetWindow() const { return m_window; }
        std::shared_ptr<SDL_Renderer> GetRenderer() const { return m_renderer; }
        
        void Clear(Uint8 r = 30, Uint8 g = 30, Uint8 b = 30, Uint8 a = 255);
        void Present();
    };
}