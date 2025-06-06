#pragma once
#include "entt.hpp"
#include "core/Components.h"
#include "CameraSystem.h"
namespace System
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;
		void Render(entt::registry& registry, SDL_Renderer& renderer);
		void RenderTileMap(Components::Tilemap& tileMap, SDL_Renderer& renderer, System::CameraSystem& cam);
	private:
		SDL_FRect m_dstRect{ 0, 0, 0, 0 };

	};
}


