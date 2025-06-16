#include <unordered_map>
#include <string>
#include <memory>
#include "MonsterModel.h"

class MonsterTypeRegistry {
private:
	std::unordered_map<std::string, std::unique_ptr<MonsterTypeDefinition>> m_monsterTypes;
	int m_nextMonsterTypeId = 0;
public:
	void RegisterMonsterType(const std::string& validMonster, const std::string& name, std::string textureName);
	MonsterTypeDefinition* GetMonsterType(const std::string& name);
	const std::unordered_map<std::string, std::unique_ptr<MonsterTypeDefinition>>& GetMonsterTypeMap() const;
	void UpdateDefaultProperties(const std::string& name, const MonsterProperties& properties);
	int GetNextMonsterTypeId();
	std::vector<MonsterTypeDefinition*> GetAllMonsterTypes();
};