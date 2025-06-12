#include "MonsterTypeRegistry.h"

void MonsterTypeRegistry::RegisterMonsterType(const std::string& name, const char* texturePath)
{
	int monsterTypeId = GetNextMonsterTypeId();
	m_monsterTypes.insert_or_assign(name, std::make_unique<MonsterTypeDefinition>(monsterTypeId,name, texturePath));
}

MonsterTypeDefinition* MonsterTypeRegistry::GetMonsterType(const std::string& name)
{
	auto it = m_monsterTypes.find(name);
	if (it!= m_monsterTypes.end())
	{
		return it->second.get();
	}
	return nullptr;
}

const std::unordered_map<std::string, std::unique_ptr<MonsterTypeDefinition>>& MonsterTypeRegistry::GetAllTypes() const
{
	return m_monsterTypes;
}

void MonsterTypeRegistry::UpdateDefaultProperties(const std::string& name, const MonsterProperties& properties)
{
	auto it = m_monsterTypes.find(name);
	if (it != m_monsterTypes.end())
	{
		it->second->defaultProperties = properties;
	}
	else
	{
		// Handle the case where the monster type does not exist
	}
}

int MonsterTypeRegistry::GetNextMonsterTypeId() 
{
	return m_nextMonsterTypeId++;
}
