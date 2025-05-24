#include "Game.h"
namespace Core
{
	Game::Game()
	{
		if (SDL_Init(SDL_INIT_VIDEO) == 0)
		{
			std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
			return;
		}

		m_window = SDL_CreateWindow(ApplicationConfig::APP_NAME,
			ApplicationConfig::DEFAULT_WINDOW_WIDTH, ApplicationConfig::DEFAULT_WINDOW_HEIGHT,
			SDL_WINDOW_RESIZABLE);
		if (!m_window)
		{
			std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
			SDL_Quit();
			return;
		}

		m_renderer = SDL_CreateRenderer(m_window, nullptr);
		if (!m_renderer)
		{
			std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << '\n';
			SDL_DestroyWindow(m_window);
			SDL_Quit();
			return;
		}
	};
	Game::~Game() = default;

	void Game::Run()
	{
		ScenePlay scenePlay;
		scenePlay.Initialize();
		m_isRunning = true;
		SDL_Event event;
		Uint32 previousTicks = SDL_GetTicks();
		float deltaTime = 0.0f;
	

		while (m_isRunning)
		{
			Uint32 currentTicks = SDL_GetTicks();
			deltaTime = (currentTicks - previousTicks) / 1000.0f;
			previousTicks = currentTicks;

			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_EVENT_QUIT)
				{
					m_isRunning = false;
				}
			}
			scenePlay.Update(deltaTime);
		
			SDL_SetRenderDrawColor(m_renderer, 30, 30, 30, 255);
			SDL_RenderClear(m_renderer);
			scenePlay.SRender(m_renderer);
			SDL_RenderPresent(m_renderer);
		}

		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
		return;

	}
}

