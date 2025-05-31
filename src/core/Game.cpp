#include "Game.h"
#include "ScenePlay.h"
#include <stdexcept>

namespace Core
{
    Game::Game()
    {
        if (SDL_Init(SDL_INIT_VIDEO) == 0)
        {
            throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
        }
        
        m_window = std::make_shared<Window>();
    }
    
    Game::~Game()
    {
        SDL_Quit();
    }
    
    void Game::Initialize()
    {
        try
        {
            // Initialize window
            m_window->Initialize(
                ApplicationConfig::APP_NAME,
                ApplicationConfig::DEFAULT_WINDOW_WIDTH,
                ApplicationConfig::DEFAULT_WINDOW_HEIGHT
            );
            
            // Initialize default scenes
            auto playScene = std::make_shared<ScenePlay>();
            playScene->SetGame(std::shared_ptr<Game>(this, [](Game*){})); // Non-owning shared_ptr
            AddScene("play", playScene);
            
            // Set initial scene
            SetActiveScene("play");
        }
        catch (const std::exception& e)
        {
            // Log and rethrow
            throw;
        }
    }

    void Game::Run()
    {
        if (!m_activeScene)
        {
            throw std::runtime_error("No active scene set. Call SetActiveScene() before Run().");
        }
        
        m_activeScene->Initialize(*m_window->GetRenderer().get());
        m_isRunning = true;
        SDL_Event event;
        Uint32 previousTicks = SDL_GetTicks();
        float deltaTime = 0.0f;
        
        const int FRAME_DELAY = 1000 / ApplicationConfig::DEFAULT_MAX_FPS;

        while (m_isRunning)
        {

            Uint32 frameStart = SDL_GetTicks();
            
            Uint32 currentTicks = SDL_GetTicks();
            deltaTime = (currentTicks - previousTicks) / 1000.0f;
            previousTicks = currentTicks;

            while (SDL_PollEvent(&event))
            {
                m_activeScene->HandleInput(event);
                if (event.type == SDL_EVENT_QUIT)
                {
                    m_isRunning = false;
                    m_window->Close();
				}
            }
            
            // Update active scene
            m_activeScene->Update(deltaTime);
            
            // Render
            m_window->Clear(30, 30, 30, 255);
            m_activeScene->Render(*m_window->GetRenderer().get());
            
            // Frame rate capping
            Uint32 frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < FRAME_DELAY)
            {
                SDL_Delay(FRAME_DELAY - frameTime);
            }
        }
    }
    
    void Game::AddScene(const std::string& name, std::shared_ptr<Scene> scene)
    {
        m_scenes[name] = scene;
    }
    
    void Game::SetActiveScene(const std::string& name)
    {
        auto it = m_scenes.find(name);
        if (it != m_scenes.end())
        {
            m_activeScene = it->second;
            m_activeSceneName = name;
        }
        else
        {
            throw std::runtime_error("Scene not found: " + name);
        }
    }
}

