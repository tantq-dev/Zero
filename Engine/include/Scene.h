#pragma once
#include "entt.hpp"
#include <memory>
#include <ResourcesManager.h>

class IRenderer2D;

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
		virtual void Render(::IRenderer2D& renderer) = 0;
		virtual void HandleInput() = 0;
		virtual void HandleUI() = 0;

		void SetGame(std::weak_ptr<Game> game) { m_game = std::move(game); }
		entt::registry& GetRegistry() { return m_Registry; }

	protected:
		
		std::weak_ptr<Game> m_game;
		entt::registry m_Registry; // Entity-Component System registry
	};
}
