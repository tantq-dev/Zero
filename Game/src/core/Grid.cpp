#include "Grid.h"
#include "Components.h"

void Grid::Initialize(int width, int height, int cellSize)
{
	m_width = width;
	m_height = height;
	m_cellSize = cellSize;
}

void Grid::CreateTileEntities(entt::registry& registry, 
                              const std::vector<std::vector<int>>& mapData,
                              uint32_t spriteSheetId)
{
	// Clear existing tile entities if any
	for (auto entity : m_tileEntities)
	{
		if (registry.valid(entity))
		{
			registry.destroy(entity);
		}
	}
	m_tileEntities.clear();

	// Create tile entities from map data
	for (size_t row = 0; row < mapData.size(); ++row)
	{
		for (size_t col = 0; col < mapData[row].size(); ++col)
		{
			int tileIndex = mapData[row][col];
			
			// Skip empty tiles (0 = empty/air)
			if (tileIndex == 0)
			{
				continue;
			}

			// Create entity for this tile
			entt::entity tileEntity = registry.create();
			
			// Add Transform2D component
			auto& transform = registry.emplace<Components::Transform2D>(tileEntity);
			transform.position.x = static_cast<float>(col * m_cellSize);
			transform.position.y = static_cast<float>(row * m_cellSize);
			transform.scale = { 1.0f, 1.0f };
			transform.rotation = 0.0f;

			// Add Sprite component
			auto& sprite = registry.emplace<Components::Sprite>(tileEntity);
			sprite.visible = true;
			sprite.layer = 0; // Background layer for tiles
			sprite.opacity = 1.0f;
			sprite.flipHorizontal = false;
			sprite.pivot = { 0.0f, 0.0f }; // Top-left pivot for tiles
			
			// Set spritesheet info
			sprite.spriteSheetId = spriteSheetId;
			sprite.frameIndex = static_cast<size_t>(tileIndex - 1); // Map uses 1-based indexing
			
			// Set source rect to match cell size
			sprite.source = { 0.0f, 0.0f, static_cast<float>(m_cellSize), static_cast<float>(m_cellSize) };

			// Store entity reference
			m_tileEntities.push_back(tileEntity);
		}
	}
}

void Grid::Clear(entt::registry& registry)
{
	// Destroy all tile entities
	for (auto entity : m_tileEntities)
	{
		if (registry.valid(entity))
		{
			registry.destroy(entity);
		}
	}
	m_tileEntities.clear();
}



