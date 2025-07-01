#pragma once

#include "core/Mesh.h"
#include <SDL3/SDL_render.h>
namespace Tool
{
	class HeightMap
	{
	public:
		void LoadHeightMap(std::shared_ptr< Core::Mesh> mesh, SDL_Renderer* renderer);
		SDL_Texture* GetTexturePtr() {
			return m_heightmapTexture;
		}

		// Get the actual model dimensions in world units
		float GetModelWidth() const { return m_maxX - m_minX; }
		float GetModelHeight() const { return m_maxY - m_minY; }

		// Get texture dimensions in pixels
		int GetTextureWidth() const { return m_width; }
		int GetTextureHeight() const { return m_height; }

		// Get model bounds
		float GetMinX() const { return m_minX; }
		float GetMaxX() const { return m_maxX; }
		float GetMinZ() const { return m_minY; }
		float GetMaxZ() const { return m_maxY; }

	private:
		float m_lowestPoint = 0;
		float m_highestPoint = 0;
		int m_width = 0;
		int m_height = 0;

		// Model bounds for coordinate mapping
		float m_minX = 0;
		float m_maxX = 0;
		float m_minY = 0;
		float m_maxY = 0;

		SDL_Texture* m_heightmapTexture;
	};
}