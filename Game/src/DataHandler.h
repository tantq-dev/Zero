#pragma once
//#include "json.hpp"
#include "MonsterModel.h"
#include "BulletModel.h"
#include "json.hpp"
#include "entt.hpp"
#include "WaveModel.h"
#include "system/GridSystem.h"
namespace Tool {
	class DataHandler {

	public:
		DataHandler() = default;
		~DataHandler() = default;

		void ExportAllToSingleJson(const std::vector<MonsterTypeDefinition*>& monsterDefinitions,
			std::vector<MonsterWave*> waveInformations,
			entt::registry& registry,
			const std::string& mapId = "Map_1",
			const std::string& mapName = "Default Map");
		void ImportAllData(System::GridSystem* gridSystem);
		void ImportFromSingleJson(System::GridSystem* gridSystem);
	private:
		void GetBulletsFromMonsters(const std::vector<MonsterTypeDefinition*>& monsterDefinitions);
		void ImportBulletsFromJson(const nlohmann::json& bulletsJson);
		void ImportMonstersFromJson(const nlohmann::json& enemiesJson);
		void ImportWavesFromJson(const nlohmann::json& wavesJson, System::GridSystem* gridSystem);

		std::vector<BulletConfig> m_bullets;
		std::vector<MonsterTypeDefinition> m_importedMonsterDefinitions;
		std::vector<MonsterWave> m_importedMonsterWave;
		std::unordered_map<std::string, BulletConfig> m_importedBulletConfigs;
		entt::registry* m_registry = nullptr; // Changed from reference to pointer with default initialization
		void GetBulletPropertiesFromMultipleBehavior(BehaviorMultiConfig* multipleBehavior);
		nlohmann::json ProcessBehaviorTree(const BehaviorConfig* behavior, const std::string& monsterId);
		std::string GetBulletID(const BulletConfig& bulletConfig);
		std::string BulletTypeToString(BulletType type);
		std::unique_ptr<BehaviorMultiConfig> ImportBehaviorTree(const nlohmann::json& behaviorJson, const std::string& monsterId);
	};
}