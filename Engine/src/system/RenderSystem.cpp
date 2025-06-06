#include "RenderSystem.h"
namespace System
{
	void RenderSystem::Render(entt::registry& registry, SDL_Renderer& renderer)
	{
		const auto view = registry.group<>(entt::get<Components::Transform, Components::Animator>);

		const SDL_FRect* srcRect = nullptr;
		for (const auto entity : view)
		{
			const auto& transform = view.get<Components::Transform>(entity);

			if (registry.all_of<Components::Animator>(entity))
			{
				auto& animator = registry.get<Components::Animator>(entity);
				const auto animation = animator.GetCurrentAnimation();
				srcRect = new SDL_FRect{
					animation->frameWidth * animation->currentFrame,
					0,
					animation->frameWidth,
					animation->frameHeight

				};
				m_dstRect.x = transform.position.x - transform.scale.x / 2;
				m_dstRect.y = transform.position.y - transform.scale.y / 2;
				m_dstRect.w = transform.scale.x;
				m_dstRect.h = transform.scale.y;

				SDL_RenderTexture(&renderer, animation->texture, srcRect, &m_dstRect);
				delete srcRect; // Clean up the dynamically allocated srcRect
			}



		}
		SDL_RenderPresent(&renderer);
	}

	void RenderSystem::RenderTileMap(Components::Tilemap& tileMap, SDL_Renderer& renderer, System::CameraSystem& cam)
	{
		// Get tilemap properties
		const int tileWidth = tileMap.GetTileWidth();
		const int tileHeight = tileMap.GetTileHeight();
		const int mapWidth = tileMap.GetMapWidth();
		const int mapHeight = tileMap.GetMapHeight();
		const int mapRows = mapHeight / tileHeight;
		const int mapCols = mapWidth / tileWidth;

		// Set color for grid lines
		SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 128);

		// Prepare vertex arrays for horizontal and vertical lines
		std::vector<SDL_FPoint> vertices;
		vertices.reserve((mapRows + 1 + mapCols + 1) * 2); // 2 points per line

		// Create horizontal grid lines
		for (int r = 0; r <= mapRows; r++) {
			const float y = r * tileHeight;
			vertices.push_back({ 0.0f + cam.GetCameraPosition().x , y + cam.GetCameraPosition().y });                // Start point
			vertices.push_back({ static_cast<float>(mapWidth) + cam.GetCameraPosition().x, y + cam.GetCameraPosition().y }); // End point
		}

		// Create vertical grid lines
		for (int c = 0; c <= mapCols; c++) {
			const float x = c * tileWidth;
			vertices.push_back({ x - cam.GetCameraPosition().x, 0.0f + cam.GetCameraPosition().y });                // Start point
			vertices.push_back({ x - cam.GetCameraPosition().x, static_cast<float>(mapHeight + cam.GetCameraPosition().y) }); // End point
		}

		// Batch render all lines in one call
		for (size_t i = 0; i < vertices.size(); i += 2) {
			SDL_RenderLine(&renderer,
				vertices[i].x, vertices[i].y,
				vertices[i + 1].x, vertices[i + 1].y);
		}

	}

}

