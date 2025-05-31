#pragma once
#include "entt.hpp"
#include <memory>
#include <SDL3/SDL.h>
#include "ResourcesManager.h""

namespace Core
{
    class Game;

    class Scene
    {
    public:
        Scene() = default;
        virtual ~Scene() = default;
        Scene(const Scene&) = delete; // Disable copy constructor
        Scene& operator=(const Scene&) = delete; // Disable copy assignment operator

        virtual void Initialize(SDL_Renderer& renderer) = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Render(SDL_Renderer& renderer) = 0;
        virtual void HandleInput(SDL_Event& event) = 0;
        
        void SetGame(std::shared_ptr<Game> game) { m_Game = game; }
        std::shared_ptr<Game> GetGame() const { return m_Game; }
        
    protected:
        std::shared_ptr<Game> m_Game; // Pointer to the Game instance
        entt::registry m_Registry; // Entity-Component System registry
    };
}
