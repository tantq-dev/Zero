#pragma once
#include "entt.hpp"
#include "Components.h"
#include "CameraSystem.h"
#include "IRenderer2D.h"
namespace System
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		RenderSystem(std::shared_ptr<IRenderer2D> renderer) : m_renderer(renderer) {}
		~RenderSystem() = default;

		void Update(entt::registry& registry);
		Components::Texture LoadTexture(const std::string& path) {
			if (m_renderer) {
				return m_renderer->GetTextureFromFile(path);
			}
			return {};
		}
	private:
		std::shared_ptr<IRenderer2D> m_renderer;
	};
}


