#pragma once
#include "entt.hpp"
#include <memory>
#include <SDL3/SDL.h>
#include <ResourcesManager.h>


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

		virtual void Initialize() = 0;
		virtual void Update(const double& deltaTime) = 0;
		virtual void FixedUpdate(const double& deltaTime) = 0;
		virtual void HandleInput(SDL_Event& event) = 0;
		virtual void HandleUI(SDL_Event& event) = 0;

		void SetGame(std::weak_ptr<Game> game) { m_game = std::move(game); }
		entt::registry& GetRegistry() { return m_Registry; }

	protected:
		std::unique_ptr<ResourcesManager> m_resources;
		std::weak_ptr<Game> m_game;
		entt::registry m_Registry; // Entity-Component System registry
	};
}
