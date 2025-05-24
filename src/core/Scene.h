#pragma once
#include "entt.hpp"
#include "Game.h"

//forward declaration of Game class
class Game;

namespace Core
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;
		Scene(const Scene&) = delete; // Disable copy constructor
		Scene& operator=(const Scene&) = delete; // Disable copy assignment operator

		virtual void Initialize() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void SRender(SDL_Renderer* render) = 0;
		virtual void SDoAction() = 0;
	private:


	protected:
		entt::registry m_Registry; // Entity-Component System registry
	};
}
