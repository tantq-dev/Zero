#pragma once
#include "entt.hpp"
#include "Components.h"
#include "IRenderer2D.h"
#include "ResourcesManager.h"
namespace System
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void Update(entt::registry& registry);
		void SetResourcesManager(ResourcesManager* resources) { m_resources = resources; }
		void SetRenderer(std::shared_ptr<IRenderer2D> renderer) { m_renderer = renderer; }
		IRenderer2D& GetRenderer() { 
			assert(m_renderer && "Renderer is null!");
			return *m_renderer; 
		}
		Components::Texture LoadTexture(const std::string& path) {
			if (m_renderer) {
				return m_renderer->GetTextureFromFile(path);
			}
			return {};
		}
	private:
		std::shared_ptr<IRenderer2D> m_renderer;
		ResourcesManager* m_resources = nullptr;
	};
}


