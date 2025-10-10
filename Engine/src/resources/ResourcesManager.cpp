#include "ResourcesManager.h"

ResourcesManager::ResourcesManager()
{


}


void ResourcesManager::StoreSpriteSheet(uint32_t id, const Components::SpriteSheet& spriteSheet)
{
	m_spriteSheet.insert_or_assign(id, spriteSheet);
}

Components::SpriteSheet* ResourcesManager::GetSpriteSheet(uint32_t id)
{
	auto it = m_spriteSheet.find(id);
	if (it != m_spriteSheet.end()) {
		return &it->second;
	}
	else {
		return nullptr;
	}
}



