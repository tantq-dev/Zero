#pragma once
#include "entt.hpp"
#include <memory>
#include <ResourcesManager.h>
#include <World.h>
#include <Actor.h>

class IRenderer2D;

namespace Core
{
	class Game;

	class Scene
	{
	public:
		Scene() {
			m_world = std::make_shared<World>();
		}
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
		std::shared_ptr<World> GetWorld()
		{
			return m_world;
		}

	protected:
		
		std::weak_ptr<Game> m_game;
		std::shared_ptr<World> m_world;
		std::vector<std::shared_ptr<Core::Actor>> m_actors;
	};
}
