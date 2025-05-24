#pragma once
#include <SDL3/SDL.h>
#include "Logger.h"
#include "ApplicationConfig.h"
#include <algorithm>
#include "ScenePlay.h"
namespace Core
{
    class Game
    {
    private:
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        bool m_isRunning = false;
    public:
        Game();
        ~Game();
        void Run();
		SDL_Window* GetWindow() const
		{
			return m_window;
		}
        SDL_Renderer* GetRenderer() const
        {
			return m_renderer;
        }
    };
}


