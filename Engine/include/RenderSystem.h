#pragma once
#include "entt.hpp"
#include "Components.h"
#include "CameraSystem.h"

namespace System
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;
		void RenderAnimation(entt::registry& registry, SDL_Renderer& renderer);
		void RenderSprite(entt::registry& registry, SDL_Renderer& renderer, System::CameraSystem& cam);
		void RenderGrid(const Components::Grid& tileMap, SDL_Renderer& renderer, System::CameraSystem& cam);

	private:
		SDL_FRect m_dstRect{ 0, 0, 0, 0 };

	};
}


