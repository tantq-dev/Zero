#include "HeightMap.h"
#include "utilities/Logger.h"
#include <SDL3/SDL_render.h>
#include <algorithm>
void Tool::HeightMap::LoadHeightMap(std::shared_ptr< Core::Mesh> mesh, SDL_Renderer* renderer)
{
	m_lowestPoint = mesh->vertices[0].position.y;
	m_highestPoint = mesh->vertices[0].position.y;

	float leftPoint = mesh->vertices[0].position.x;
	float rightPoint = mesh->vertices[0].position.x;

	float topPoint = mesh->vertices[0].position.z;
	float bottomPoint = mesh->vertices[0].position.z;

	for (size_t i = 0; i < mesh->vertices.size(); i++)
	{
		float y = mesh->vertices[i].position.y;
		float x = mesh->vertices[i].position.x;
		float z = mesh->vertices[i].position.z;

		if (m_lowestPoint > z) {
			m_lowestPoint = z;
		}
		if (m_highestPoint < z) {
			m_highestPoint = z;
		}

		if (bottomPoint > y) {
			bottomPoint = y;
		}

		if (topPoint < y) {
			topPoint = y;
		}

		if (leftPoint > x) {
			leftPoint = x;
		}

		if (rightPoint < x) {
			rightPoint = x;
		}
	}

	// Calculate the actual dimensions of the model
	float modelWidth = abs(rightPoint - leftPoint);
	float modelHeight = abs(topPoint - bottomPoint);

	// Store the model bounds for later use
	m_minX = leftPoint;
	m_maxX = rightPoint;
	m_minY = bottomPoint;
	m_maxY = topPoint;

	// Set texture resolution (you can adjust this based on your needs)
	// Higher resolution = more detail but larger texture
	float texelsPerUnit = 50; // Adjust this value as needed

	m_width = static_cast<int>(modelWidth * texelsPerUnit);
	m_height = static_cast<int>(modelHeight * texelsPerUnit);

	// Ensure minimum dimensions
	if (m_width < 1) m_width = 1;
	if (m_height < 1) m_height = 1;

	// Ensure reasonable maximum dimensions to avoid memory issues
	const int MAX_TEXTURE_SIZE = 4096;
	if (m_width > MAX_TEXTURE_SIZE) m_width = MAX_TEXTURE_SIZE;
	if (m_height > MAX_TEXTURE_SIZE) m_height = MAX_TEXTURE_SIZE;

	LOG_INFO("Model dimensions - Width: " + std::to_string(modelWidth) + ", Height: " + std::to_string(modelHeight));
	LOG_INFO("Texture dimensions - W: " + std::to_string(m_width) + ", H: " + std::to_string(m_height));


	m_heightmapTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		m_width, m_height
	);

	std::vector<uint32_t> pixels(m_width * m_height, 0);

	for (const auto& vertex : mesh->vertices) {
		// Map 3D world coordinates to 2D texture coordinates
		float normalizedX = (vertex.position.x - m_minX) / (m_maxX - m_minX);
		float normalizedY = (vertex.position.y - m_minY) / (m_maxY - m_minY);

		// Convert normalized coordinates to pixel coordinates
		int x = static_cast<int>(normalizedX * (m_width - 1));
		int y = static_cast<int>(normalizedY * (m_height - 1));

		// Clamp coordinates to valid range
		if (x < 0) x = 0;
		if (x >= m_width) x = m_width - 1;
		if (y < 0) y = 0;
		if (y >= m_height) y = m_height - 1;

		// Normalize height to 0-1 range
		float normalizedHeight = (vertex.position.y - m_lowestPoint) / (m_highestPoint - m_lowestPoint);
		// Convert to grayscale (0-255)
		uint8_t gray = static_cast<uint8_t>(normalizedHeight * 255.0f);
		// Create RGBA value (grayscale)
		uint32_t pixel = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;

		// Calculate pixel index safely
		size_t pixelIndex = static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x);
		if (pixelIndex < pixels.size()) {
			// Use the highest height value if multiple vertices map to same pixel
			uint32_t existingPixel = pixels[pixelIndex];
			uint8_t existingGray = existingPixel & 0xFF;
			if (gray > existingGray) {
				pixels[pixelIndex] = pixel;
			}
		}
		else {
			LOG_ERROR("Pixel index out of bounds: " + std::to_string(pixelIndex) + " >= " + std::to_string(pixels.size()));
		}
	}

	// Update the texture with our pixel data
	void* texturePixels;
	int pitch;
	if (SDL_LockTexture(m_heightmapTexture, nullptr, &texturePixels, &pitch) < 0) {
		LOG_ERROR("Fail to lock texture");
		return;
	}

	// Copy our pixel data to the texture
	memcpy(texturePixels, pixels.data(), pixels.size() * sizeof(uint32_t));

	// Unlock the texture
	SDL_UnlockTexture(m_heightmapTexture);

}



