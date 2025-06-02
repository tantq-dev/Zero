#pragma once
#include <SDL3/SDL.h>
#include "../utilities/Logger.h"
#include "../config/ApplicationConfig.h"
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include "Window.h"
#include "Scene.h"

namespace Core
{
    class Game
    {
    private:
        std::shared_ptr<Window> m_window;
        bool m_isRunning = false;
        std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
        std::shared_ptr<Scene> m_activeScene;
        std::string m_activeSceneName;
        
    public:
        Game();
        ~Game();
        
        void Initialize();
        void Run();
        
        std::shared_ptr<Window> GetWindow() const { return m_window; }
        
        // Scene management
        void AddScene(const std::string& name, std::shared_ptr<Scene> scene);
        void SetActiveScene(const std::string& name);
        std::shared_ptr<Scene> GetActiveScene() const { return m_activeScene; }
        std::string GetActiveSceneName() const { return m_activeSceneName; }
    };
}


