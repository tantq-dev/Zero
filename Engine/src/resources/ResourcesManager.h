#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <unordered_map> // Use unordered_map for better performance on lookups
class ResourcesManager
{
	public:
	~ResourcesManager() = default;
	// Get resource by name
	SDL_Texture* GetTexture(const std::string& name);
	void LoadTexture(const std::string& name, const char* path, SDL_Renderer * renderer);
	static ResourcesManager& GetInstance();
private:
	ResourcesManager();
	static ResourcesManager* m_instance;
	std::unordered_map<std::string, SDL_Texture*> m_textures; // Vector to hold loaded textures
};

