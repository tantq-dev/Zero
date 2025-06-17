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
		void ExportBulletConfig(const std::vector<MonsterTypeDefinition*>& monsterDefinitions);
		void ImportAllData(System::GridSystem* gridSystem);
		void ExportPositionJson(std::vector<MonsterWave*> m_pWaveInformations, entt::registry& registry);
	private:
		std::vector<BulletConfig> m_bullets;
		std::vector<MonsterTypeDefinition> m_importedMonsterDefinitions;
		std::vector<MonsterWave> m_importedMonsterWave;
		std::unordered_map<std::string, BulletConfig> m_importedBulletConfigs;
		void GetBulletPropertiesFromMultipleBehavior(BehaviorMultiConfig* multipleBehavior);
		void ExportToJson();
		void ExportMonsterToJson(const std::vector<MonsterTypeDefinition*>& monsterDefinitions);
		void ImportMonsterFromJson();
		void ImportBulletFromJson();
		void ImportMonsterWavesFromJson(System::GridSystem* gridSystem);
		nlohmann::json ProcessBehaviorTree(const BehaviorConfig* behavior, const std::string& monsterId);
		std::string GetBulletID(const BulletConfig& bulletConfig);
		std::string BulletTypeToString(BulletType type);
		std::unique_ptr<BehaviorMultiConfig> ImportBehaviorTree(const nlohmann::json& behaviorJson, const std::string& monsterId);
	};
}