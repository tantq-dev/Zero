#include "RenderSystem.h"
#include "config/ApplicationConfig.h"
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

	}

	void RenderSystem::RenderGrid(const Components::Grid& tileMap, SDL_Renderer& renderer, System::CameraSystem& cam)
	{
		float tileSize = tileMap.GetCellSize() * cam.GetCameraZoom();
		const float mapRows = tileMap.GetHeight();
		const float mapCols = tileMap.GetWidth();


		// Calculate base position for the tilemap
		const float baseX = 0;
		const float baseY = 0;

		Vec2 gridWorldPos = { baseX, baseY };
		Vec2 cameraPos = { 0,0 };
		cam.WorldToCameraView(gridWorldPos, cameraPos);




		// Set color for grid lines
		SDL_SetRenderDrawColor(&renderer, 100, 100, 100, 128);

		// Prepare vertex arrays for horizontal and vertical lines
		std::vector<SDL_FPoint> vertices;
		vertices.reserve((mapRows + 1 + mapCols + 1) * 2); // 2 points per line

		// Create horizontal grid lines
		for (int r = 0; r <= mapRows; r++) {
			const float y = r * tileSize;
			vertices.push_back({ cameraPos.x, y + cameraPos.y });                // Start point
			vertices.push_back({ cameraPos.x + static_cast<float>(mapCols * tileSize), y + cameraPos.y }); // End point
		}

		// Create vertical grid lines
		for (int c = 0; c <= mapCols; c++) {
			const float x = c * tileSize;
			vertices.push_back({ x + cameraPos.x, cameraPos.y });                // Start point
			vertices.push_back({ x + cameraPos.x, cameraPos.y + static_cast<float>(mapRows * tileSize) }); // End point
		}

		// Batch render all lines in one call
		for (size_t i = 0; i < vertices.size(); i += 2) {
			SDL_RenderLine(&renderer,
				vertices[i].x, vertices[i].y,
				vertices[i + 1].x, vertices[i + 1].y);
		}

		//// First, render all tiles with appropriate colors
		for (int r = 0; r < mapRows; r++) {
			for (int c = 0; c < mapCols; c++) {

				int tileIndex = r * static_cast<int>(mapCols) + c;
				// Create tile rectangle
				SDL_FRect tileRect = {
					cameraPos.x + (c * tileSize + tileSize * 0.01),
					cameraPos.y + (r * tileSize + tileSize * 0.01),
					tileSize ,
					tileSize
				};

				//// Fill all tiles with the base gray color
				SDL_SetRenderDrawColor(&renderer, 50, 50, 50, 255);
				SDL_RenderRect(&renderer, &tileRect);

				// Draw colored border for highlighted tiles
				if (tileMap.cells[tileIndex].isColor) {
					SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 255);
					SDL_RenderRect(&renderer, &tileRect);  // Draw just the outline
				}
			}
		}
	}

}

