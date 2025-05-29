#pragma once
#include "entt.hpp"
#include "Components.h"
namespace System
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;
		void Render(entt::registry& registry, SDL_Renderer& renderer);
	private:
		SDL_FRect m_dstRect{ 0, 0, 0, 0 };

	};
}


