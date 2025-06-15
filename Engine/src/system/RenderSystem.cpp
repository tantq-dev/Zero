#include "RenderSystem.h"
#include "config/ApplicationConfig.h"
namespace System
{
	void RenderSystem::RenderAnimation(entt::registry& registry, SDL_Renderer& renderer)
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


	void RenderSystem::RenderSprite(entt::registry& registry, SDL_Renderer& renderer, System::CameraSystem& cam)
	{
		const auto view = registry.group<>(entt::get<Components::Transform, Components::Sprite>);

		for (const auto entity : view)
		{
			const auto& transform = view.get<Components::Transform>(entity);

			if (registry.all_of<Components::Sprite>(entity))
			{
				auto& sprite = registry.get<Components::Sprite>(entity);
				if (!sprite.isVisible) continue;
				Vec2 cameraPos = { 0,0 };
				cam.WorldToCameraView(transform.position, cameraPos);

				m_dstRect.x = cameraPos.x - transform.scale.x / 2 * cam.GetCameraZoom(); // Center the sprite should caculate from transform.position
				m_dstRect.y = cameraPos.y - transform.scale.y / 2 * cam.GetCameraZoom(); // Center the sprite should caculate from transform.position
				m_dstRect.w = transform.scale.x * cam.GetCameraZoom();
				m_dstRect.h = transform.scale.y * cam.GetCameraZoom();
				SDL_RenderTexture(&renderer, sprite.texture, nullptr, &m_dstRect);
			}
		}

	}


	void RenderSystem::RenderGrid(const Components::Grid& grid, SDL_Renderer& renderer, System::CameraSystem& cam)
	{
		float tileSize = grid.GetCellSize() * cam.GetCameraZoom();
		const float mapRows = grid.GetHeight();
		const float mapCols = grid.GetWidth();


		const float baseX = -(mapCols * grid.GetCellSize()) / 2.0f;
		const float baseY = -(mapRows * grid.GetCellSize()) / 2.0f;




		Vec2 gridWorldPos = { baseX, baseY };
		Vec2 cameraPos = { 0,0 };
		cam.WorldToCameraView(gridWorldPos, cameraPos);

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
				if (grid.cells[tileIndex].isColor) {
					SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 255);
					SDL_RenderRect(&renderer, &tileRect);  // Draw just the outline
				}
			}
		}
	}

}

