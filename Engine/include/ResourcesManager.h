#pragma once
#include <string>
#include <unordered_map> // Use unordered_map for better performance on lookups
#include "Components.h"

class ResourcesManager
{
public:
	~ResourcesManager() = default;
	ResourcesManager();
	// Get resource by name
	static ResourcesManager& GetInstance();
	void StoreSpriteSheet(uint32_t id, const Components::SpriteSheet& spriteSheet);
	Components::SpriteSheet* GetSpriteSheet(uint32_t id);
private:
	static ResourcesManager* m_instance;
	std::unordered_map <uint32_t, Components::SpriteSheet> m_spriteSheet; 
};

