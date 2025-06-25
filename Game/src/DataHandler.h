#pragma once
//#include "json.hpp"
#include "MonsterModel.h"
#include "json.hpp"
#include "entt.hpp"
#include "WaveModel.h"
#include "system/GridSystem.h"
#include <unordered_set>
#include "BulletRegistry.h"
namespace Tool {
	class DataHandler {

	public:
		DataHandler();
		~DataHandler() = default;

		void ExportAllToSingleJson(const std::vector<MonsterTypeDefinition*>& monsterDefinitions,
			std::vector<MonsterWave*> waveInformations,
			entt::registry& registry,
			const std::string& mapId = "Map_1",
			const std::string& mapName = "Default Map");
		void ImportAllData(System::GridSystem* gridSystem);
		void ImportFromSingleJson(System::GridSystem* gridSystem);
		void SetBulletRegistry( BulletRegistry* bulletRegistry) { m_bulletRegistry = bulletRegistry; }
	private:
		void GetBulletsFromMonsters(const std::vector<MonsterTypeDefinition*>& monsterDefinitions);
		void ImportBulletsFromJson(const nlohmann::json& bulletsJson);
		void ImportMonstersFromJson(const nlohmann::json& enemiesJson);
		void ImportWavesFromJson(const nlohmann::json& wavesJson, System::GridSystem* gridSystem);
		std::unordered_set<std::string> m_bulletIds;
		std::vector<MonsterTypeDefinition> m_importedMonsterDefinitions;
		std::vector<MonsterWave> m_importedMonsterWave;
		std::unordered_map<std::string, BulletConfig> m_importedBulletConfigs;
		entt::registry* m_registry = nullptr;
		BulletRegistry* m_bulletRegistry = nullptr;
		void GetBulletPropertiesFromMultipleBehavior(BehaviorMultiConfig* multipleBehavior);
		nlohmann::json ProcessBehaviorTree(const BehaviorConfig* behavior, const std::string& monsterId);
		std::unique_ptr<BehaviorMultiConfig> ImportBehaviorTree(const nlohmann::json& behaviorJson, const std::string& monsterId);

	};
}