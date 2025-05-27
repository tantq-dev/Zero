#include "ResourcesManager.h"

SDL_Texture* ResourcesManager::GetTexture(const std::string& name)
{
	return m_textures[name];
}

void ResourcesManager::LoadTexture(const std::string& name, const char* path, SDL_Renderer* renderer)
{
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	SDL_Texture* texture = NULL;
	SDL_asprintf(&bmp_path, path, SDL_GetBasePath());

	// Load BMP into surface
	surface = SDL_LoadBMP(bmp_path);
	if (!surface)
	{
		SDL_free(bmp_path);
		return;
	}
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture)
	{
		SDL_Log("Texture creation failed: %s", SDL_GetError());
		SDL_DestroySurface(surface);

	}
	SDL_DestroySurface(surface);
	SDL_free(bmp_path);

	m_textures[name] = texture;
}

ResourcesManager& ResourcesManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new ResourcesManager();
	}
	return *m_instance;
}
