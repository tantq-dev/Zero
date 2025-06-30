#include "DataHandler.h"
#include "utilities/Logger.h"
#include "MonsterModel.h"
#include "BulletRegistry.h"
#include <memory>
#include <fstream>
#include <unordered_set>
#include "core/EventSystem.h"
#include "EventKey.h"
#include <WaveModel.h>
#include "DataHandlerRegistry.h"


Tool::DataHandler::DataHandler()
{
}

void Tool::DataHandler::GetBulletPropertiesFromMultipleBehavior(BehaviorMultiConfig* multipleBehavior)
{
	for (auto& behavior : multipleBehavior->childBehaviors)
	{
		if (auto castedBehavior = dynamic_cast<BehaviorMultiConfig*>(behavior.get()))
		{
			GetBulletPropertiesFromMultipleBehavior(castedBehavior);
		}
		else if (auto castedBehavior = dynamic_cast<BehaviorShootBarrageConfig*>(behavior.get()))
		{
			if (!castedBehavior->bulletId.empty()) {
				m_bulletIds.insert(castedBehavior->bulletId);
			}
		}
		else if (auto castedBehavior = dynamic_cast<BehaviorShootProjectileConfig*>(behavior.get()))
		{
			if (!castedBehavior->bulletId.empty()) {
				m_bulletIds.insert(castedBehavior->bulletId);
			}
		}
		else if (auto castedBehavior = dynamic_cast<BehaviorSpreadShotConfig*>(behavior.get()))
		{
			if (!castedBehavior->bulletId.empty()) {
				m_bulletIds.insert(castedBehavior->bulletId);
			}
		}
	}
}


void Tool::DataHandler::ExportAllToSingleJson(const std::vector<MonsterTypeDefinition*>& monsterDefinitions, std::vector<MonsterWave*> waveInformations, entt::registry& registry, BulletRegistry& bulletRegistry,const std::string& mapId, const std::string& mapName)
{
	m_registry = &registry; // Store the registry pointer
	LOG_INFO("Exporting all data to JSON file");

	nlohmann::json rootJson;

	//// Process bullets
	//GetBulletsFromMonsters(monsterDefinitions);
	m_bulletRegistry = &bulletRegistry;

	// Enemies section (from ExportMonsterToJson)
	nlohmann::json enemiesJson;

	for (auto monsterDef : monsterDefinitions)
	{
		if (!monsterDef) continue;

		std::string monsterId = "enemy_" + std::to_string(monsterDef->item.id);

		nlohmann::json monsterJson;
		monsterJson["ID"] = monsterId;
		monsterJson["Name"] = monsterDef->defaultProperties.name;
		monsterJson["AssetID"] = monsterDef->defaultProperties.valideMonsterIngame;
		monsterJson["MaxHealth"] = monsterDef->defaultProperties.hp;

		// Convert from internal integer format to float
		float knockbackResistance = monsterDef->defaultProperties.knockbackResistance;
		float moveSpeed = monsterDef->defaultProperties.speed;

		monsterJson["KnockBackResistance"] = knockbackResistance;
		monsterJson["MoveSpeed"] = moveSpeed;
		monsterJson["EnemyType"] = (monsterDef->defaultProperties.monsterType == MonsterType::Boss) ? "BOSS" : "NORMAL";
		monsterJson["CollisionDamage"] = monsterDef->defaultProperties.collisionDamage;
		monsterJson["Defense"] = 0;
		monsterJson["DodgeRate"] = 0;

		// Process behavior tree
		if (monsterDef->defaultProperties.rootBehavior) {
			monsterJson["BehaviorConfig"] = ProcessBehaviorTree(monsterDef->defaultProperties.rootBehavior.get(), monsterId);
		}

		enemiesJson[monsterId] = monsterJson;
	}

	rootJson["enemies"] = enemiesJson;

	// Bullets section - Get actual bullet definitions for the IDs used in behaviors
	nlohmann::json bulletsJson;
	for (auto& bulletDef : m_bulletRegistry->GetBulletTypeMap())
	{
		nlohmann::json bulletJson;
		
		// Try to get bullet data from bullet registry if available
	/*	if (m_bulletRegistry) {
			bulletDef = m_bulletRegistry->GetBulletType(bulletId);
		}*/
		auto& bulletId = bulletDef.first;
		
		if (bulletDef.second) {
			// Use actual bullet data from registry
			bulletJson["ID"] = bulletDef.second->config.ID;
			bulletJson["AssetID"] = bulletDef.second->config.validBulletIngame;
			bulletJson["MoveSpeed"] = bulletDef.second->config.speed;
			bulletJson["Damage"] = bulletDef.second->config.damage;
			bulletJson["AliveTime"] = bulletDef.second->config.aliveTime;
			bulletJson["Elemental"] = "";
			bulletJson["Bounce"] = bulletDef.second->config.bounce;
			nlohmann::json moveBehavior;
			moveBehavior["type"] = BulletTypeToString(bulletDef.second->config.bulletType);
			bulletJson["MoveBehavior"] = moveBehavior;
			auto spawnerBulletPtr = bulletDef.second->config.spawnerBullet.get();
			if (spawnerBulletPtr != nullptr) {
				auto spawnerBulletJson = spawnerBulletPtr->SerializeSpecific("");
				bulletJson["SpawnerBullet"] = spawnerBulletJson;
			}
		} else {
			// Fallback to default values if bullet not found in registry
			bulletJson["AssetID"] = "bullet_01";
			bulletJson["MoveSpeed"] = 100;
			bulletJson["Damage"] = 10;
			bulletJson["AliveTime"] = 60;
			bulletJson["Elemental"] = "";
			bulletJson["Bounce"] = 0;

			nlohmann::json moveBehavior;
			// Try to determine bullet type from ID
			if (bulletDef.second->config.bulletType == BulletType::Straight) {
				moveBehavior["type"] = "Straight";
			} else if (bulletDef.second->config.bulletType == BulletType::Parabol) {
				moveBehavior["type"] = "Parabol";
			} else if (bulletDef.second->config.bulletType == BulletType::Mortal) {
				moveBehavior["type"] = "Mortal";
			} else if (bulletDef.second->config.bulletType == BulletType::Boss) {
				moveBehavior["type"] = "Boss";
			} else {
				moveBehavior["type"] = "Straight";
			}
			bulletJson["MoveBehavior"] = moveBehavior;
			
			LOG_INFO("Warning: Bullet definition not found for ID: " + bulletDef.first + ", using defaults");
		}

		bulletsJson[bulletId] = bulletJson;
	}

	rootJson["bullets"] = bulletsJson;

	// Maps section
	nlohmann::json mapsJson;
	nlohmann::json mapJson;

	mapJson["mapId"] = mapId;
	mapJson["mapName"] = mapName;

	// Waves section (from ExportPositionJson)
	nlohmann::json wavesJson = nlohmann::json::array();
	for (auto wavePtr : waveInformations)
	{
		if (!wavePtr) continue;

		nlohmann::json waveJson;
		waveJson["waveIndex"] = wavePtr->waveIndex;
		bool hasBoss = false;
		// Monsters in this wave
		nlohmann::json monstersJson = nlohmann::json::array();
		for (auto entityId : wavePtr->monsterEntities)
		{
			if (!m_registry->valid(entityId)) continue;

			auto* placedMonster = m_registry->try_get<PlacedMonster>(entityId);
			if (!placedMonster) continue;

			nlohmann::json monsterJson;
			std::string monsterId = "enemy_" + std::to_string(placedMonster->item.id);
			monsterJson["monsterId"] = monsterId;
			monsterJson["gridPosition"] = {
				{"x", placedMonster->positionInGrid.x},
				{"y", placedMonster->positionInGrid.y}
			};

			monstersJson.push_back(monsterJson);
			int id = placedMonster->item.id;
			for (auto* monsterDef : monsterDefinitions)
			{
				if (monsterDef && monsterDef->item.id == id)
				{
					if (monsterDef->defaultProperties.monsterType == MonsterType::Boss)
					{
						hasBoss = true;
					}
					break;
				}
			}
		}

		waveJson["isBossWave"] = hasBoss;
		waveJson["monsters"] = monstersJson;
		wavesJson.push_back(waveJson);
	}

	mapJson["waves"] = wavesJson;
	mapsJson[mapId] = mapJson;
	rootJson["maps"] = mapsJson;

	// Show file save dialog
	std::string filePath = "";

	// Using Windows file dialog
#ifdef _WIN32
	OPENFILENAMEA ofn;
	char szFile[260] = "gamedata.json"; // Default filename

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = "json";

	if (GetSaveFileNameA(&ofn)) {
		filePath = ofn.lpstrFile;
	}
#else
	// Fallback to default path if not on Windows
	filePath = "gamedata.json";
#endif

	if (filePath.empty()) {
		LOG_INFO("File save canceled, using default filename");
		filePath = "gamedata.json";
	}

	// Write to user-selected file
	std::ofstream outFile(filePath);
	if (outFile.is_open())
	{
		outFile << rootJson.dump(2); // Pretty print with 2-space indentation
		outFile.close();
		LOG_INFO("Successfully exported all data to: " + filePath);
	}
	else
	{
		LOG_ERROR("Failed to open file for writing: " + filePath);
	}
}

void Tool::DataHandler::ImportFromSingleJson(System::GridSystem* gridSystem)
{

	LOG_INFO("Importing data from JSON file");

	// Clear any previously imported data
	m_importedMonsterDefinitions.clear();
	m_importedBulletConfigs.clear();
	m_importedMonsterWave.clear();

	// Show file open dialog
	std::string filePath = "";

	// Using Windows file dialog
#ifdef _WIN32
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn)) {
		filePath = ofn.lpstrFile;
	}
#else
	// Fallback to default path if not on Windows (could implement other platforms)
	filePath = "gamedata.json";
#endif

	if (filePath.empty()) {
		LOG_INFO("File selection canceled");
		return;
	}

	std::ifstream inFile(filePath);
	if (!inFile.is_open()) {
		LOG_ERROR("Failed to open file: " + filePath);
		return;
	}

	try {
		// Parse the JSON file
		nlohmann::json rootJson;
		inFile >> rootJson;
		inFile.close();

		// Import bullets first (needed for behavior trees)
		if (rootJson.contains("bullets")) {
			ImportBulletsFromJson(rootJson["bullets"]);
		}

		// Import enemies
		if (rootJson.contains("enemies")) {
			ImportMonstersFromJson(rootJson["enemies"]);
		}

		// Import waves from first map
		if (rootJson.contains("maps") && !rootJson["maps"].empty()) {
			auto firstMap = rootJson["maps"].begin().value();
			if (firstMap.contains("waves")) {
				ImportWavesFromJson(firstMap["waves"], gridSystem);
			}
		}

		// Notify other systems of the imported data
		Core::EventData evData;
		evData.data = m_importedMonsterDefinitions;
		Core::EventSystem::getInstance().publish(EventKeys::SendMonsterData, evData);

		LOG_INFO("All data imported successfully from: " + filePath);
	}
	catch (const std::exception& e) {
		LOG_ERROR("Error parsing JSON file: " + std::string(e.what()));
	}
}


void Tool::DataHandler::GetBulletsFromMonsters(const std::vector<MonsterTypeDefinition*>& monsterDefinitions)
{
	m_bulletIds.clear();
	for (auto monsterDef : monsterDefinitions)
	{
		if (monsterDef) {
			GetBulletPropertiesFromMultipleBehavior(monsterDef->defaultProperties.rootBehavior.get());
		}
	}
}

void Tool::DataHandler::ImportBulletsFromJson(const nlohmann::json& bulletsJson)
{
	m_importedBulletConfigs.clear();
	m_importedBulletConfigDefinitions.clear();
	LOG_INFO("Importing bullets");

	try {
		// Process each bullet in the JSON
		int indexer = 0;
		for (auto it = bulletsJson.begin(); it != bulletsJson.end(); ++it) {
			const std::string& bulletId = it.key();
			const auto& bulletJson = it.value();

			BulletDefinition bullDefinition;

			// Create a new bullet config
			BulletConfig bulletConfig;

			// Set the valid bullet ingame identifier
			bulletConfig.validBulletIngame = bulletJson.value("AssetID", BulletTextureMap.begin()->first);

			// Set properties from JSON
			bulletConfig.speed = bulletJson.value("MoveSpeed", 0);
			bulletConfig.damage = bulletJson.value("Damage", 0);
			bulletConfig.aliveTime = bulletJson.value("AliveTime", 0);
			bulletConfig.bounce = bulletJson.value("Bounce", 0);
			bulletConfig.ID = indexer;
			// Determine bullet type from MoveBehavior
			std::string moveBehaviorType = "";
			if (bulletJson.contains("MoveBehavior") && bulletJson["MoveBehavior"].contains("type")) {
				moveBehaviorType = bulletJson["MoveBehavior"]["type"];
				bulletConfig.bulletType = StringToBulletType(moveBehaviorType);
			}
			else {
				bulletConfig.bulletType = BulletType::Straight; //default
			}

			if (bulletJson.contains("SpawnerBullet") && bulletJson["SpawnerBullet"].is_object()) {
				bulletConfig.spawnerBullet = std::make_unique<SpawnerBulletConfig>();
				bulletConfig.spawnerBullet->DeserializeSpecific(bulletJson["SpawnerBullet"], "");

				bullDefinition.config.spawnerBullet = std::make_unique<SpawnerBulletConfig>();
				bullDefinition.config.spawnerBullet->DeserializeSpecific(bulletJson["SpawnerBullet"], "");
			}
			// Add to imported bullet configs
			m_importedBulletConfigs[bulletId] = bulletConfig;
			bullDefinition.config.ID = bulletConfig.ID;
			bullDefinition.config.aliveTime = bulletConfig.aliveTime;
			bullDefinition.config.bulletType = bulletConfig.bulletType;
			bullDefinition.config.validBulletIngame = bulletConfig.validBulletIngame;
			bullDefinition.config.speed = bulletConfig.speed;
			bullDefinition.config.damage = bulletConfig.damage;
			bullDefinition.config.bounce = bulletConfig.bounce;

			bullDefinition.name = std::to_string(bulletConfig.ID);
			bullDefinition.id = indexer;
			m_importedBulletConfigDefinitions.push_back(std::move(bullDefinition));
			indexer++;

		}
		// Notify other systems of the imported data
		Core::EventData evData;
		Core::EventSystem::getInstance().publish(EventKeys::SendBulletData, evData);

		LOG_INFO("Successfully imported " + std::to_string(m_importedBulletConfigs.size()) + " bullets");
	}
	catch (const std::exception& e) {
		LOG_ERROR("Error parsing bullets: " + std::string(e.what()));
	}
}

void Tool::DataHandler::ImportMonstersFromJson(const nlohmann::json& enemiesJson)
{
	m_importedMonsterDefinitions.clear();

	LOG_INFO("Importing monsters");

	try {
		// Process each enemy in the JSON
		for (auto it = enemiesJson.begin(); it != enemiesJson.end(); ++it) {
			const auto& enemyJson = it.value();

			// Create a new monster definition
			MonsterTypeDefinition monsterDef;
			std::string id = enemyJson.value("ID", "enemy_0");

			// Set basic properties
			monsterDef.item.id = std::stoi(id.substr(6)); // Extract numeric ID from "enemy_XX"
			monsterDef.item.name = enemyJson.value("Name", "Unknown");

			std::string assetID = enemyJson.value("AssetID", "Slime");
			// Set texture name based on the type in the JSON
			std::string validMonsterType = "";
			for (const auto& pair : MonsterTextureMap) {
				if (pair.first == assetID) {
					validMonsterType = pair.first;
					monsterDef.textureName = pair.first;
					break;
				}
			}

			// If no matching texture found, use default
			if (monsterDef.textureName.empty()) {
				monsterDef.textureName = "assets//Slug.bmp"; // Default texture
			}

			// Set default properties
			monsterDef.defaultProperties.name = monsterDef.item.name;
			monsterDef.defaultProperties.valideMonsterIngame = validMonsterType;

			// Convert JSON stats to internal format
			monsterDef.defaultProperties.hp = enemyJson.value("MaxHealth", 10000);

			// Convert float values to internal integer format (multiply by 10000)
			float knockback = enemyJson.value("KnockBackResistance", 0.5f);
			monsterDef.defaultProperties.knockbackResistance = knockback;

			float speed = enemyJson.value("MoveSpeed", 1.5f);
			monsterDef.defaultProperties.speed = speed;

			// Set monster type
			std::string enemyType = enemyJson.value("EnemyType", "NORMAL");
			monsterDef.defaultProperties.monsterType = (enemyType == "BOSS") ? MonsterType::Boss : MonsterType::Normal;

			monsterDef.defaultProperties.collisionDamage = enemyJson.value("CollisionDamage", 10);

			// Process behavior tree if present
			if (enemyJson.contains("BehaviorConfig")) {
				monsterDef.defaultProperties.rootBehavior = ImportBehaviorTree(enemyJson["BehaviorConfig"], it.key());
			}
			else {
				monsterDef.defaultProperties.rootBehavior = std::make_unique<BehaviorMultiConfig>();
			}

			// Add to imported monster definitions
			m_importedMonsterDefinitions.push_back(std::move(monsterDef));
		}

		LOG_INFO("Successfully imported " + std::to_string(m_importedMonsterDefinitions.size()) + " monsters");
	}
	catch (const std::exception& e) {
		LOG_ERROR("Error parsing monsters: " + std::string(e.what()));
	}
}

void Tool::DataHandler::ImportWavesFromJson(const nlohmann::json& wavesJson, System::GridSystem* gridSystem)
{
	m_importedMonsterWave.clear();

	LOG_INFO("Importing monster waves");

	try {
		// Process each wave in the JSON
		for (const auto& waveJson : wavesJson) {
			MonsterWave wave;

			// Set wave properties
			wave.waveIndex = waveJson.value("waveIndex", 0);
			wave.isBossWave = waveJson.value("isBossWave", false);

			// Add to imported waves
			m_importedMonsterWave.push_back(wave);

			// Prepare and publish event data for monster placement
			if (waveJson.contains("monsters") && waveJson["monsters"].is_array()) {
				const auto& monstersJson = waveJson["monsters"];
				std::vector<PlacedMonster> monstersForWave;

				// Process each monster in the JSON array
				for (const auto& monsterJson : monstersJson) {
					// Extract monster ID and find corresponding definition
					std::string monsterId = monsterJson.value("monsterId", "");

					// Find the matching monster definition
					int monsterNumericId = -1;
					if (monsterId.find("enemy_") == 0) {
						try {
							monsterNumericId = std::stoi(monsterId.substr(6));
						}
						catch (const std::exception& e) {
							LOG_ERROR("Failed to parse monster ID: " + monsterId);
						}
					}

					// Find the monster definition with this ID
					MonsterTypeDefinition* matchingDef = nullptr;
					for (auto& def : m_importedMonsterDefinitions) {
						if (def.item.id == monsterNumericId) {
							matchingDef = &def;
							break;
						}
					}

					if (matchingDef) {
						// Create a PlacedMonster from the definition
						PlacedMonster placedMonster(matchingDef->item);
						placedMonster.properties = matchingDef->defaultProperties;
						placedMonster.properties.valideMonsterIngame = matchingDef->textureName;

						// Extract grid position
						if (monsterJson.contains("gridPosition")) {
							placedMonster.positionInGrid.x = monsterJson["gridPosition"].value("x", 0.0f);
							placedMonster.positionInGrid.y = monsterJson["gridPosition"].value("y", 0.0f);
							placedMonster.worldPosition = gridSystem->CellIndexInGridToWorldPosition(placedMonster.positionInGrid);
						}

						// Add to the list
						monstersForWave.push_back(placedMonster);
					}
					else {
						LOG_ERROR("Could not find monster definition for ID: " + monsterId);
					}
				}

				Core::EventData eventData;
				eventData.data = monstersForWave;
				Core::EventSystem::getInstance().publish(EventKeys::ImportMonsterWaveData, eventData);
			}
		}
	}
	catch (const std::exception& e) {
		LOG_ERROR("Error parsing waves: " + std::string(e.what()));
	}
}



// Helper function to recursively process behavior tree
nlohmann::json Tool::DataHandler::ProcessBehaviorTree(const BehaviorConfig* behavior, const std::string& monsterId)
{
	LOG_INFO("Process bahvior type: " + behavior->behaviorType);
	// Get the processor from the registry
	auto processor = BehaviorRegistry::Instance().GetProcessor(behavior->behaviorType);
	if (processor) {
		return processor(behavior, monsterId);
	}

	// Fallback if processor not found
	nlohmann::json behaviorJson;
	behaviorJson["type"] = "Unknown";
	behaviorJson["behaviorType"] = behavior->behaviorType;
	return behaviorJson;
}


void Tool::DataHandler::ImportAllData(System::GridSystem* gridSystem)
{
	ImportFromSingleJson(gridSystem);

}

// Helper function to recursively import behavior tree from JSON
std::unique_ptr<BehaviorMultiConfig> Tool::DataHandler::ImportBehaviorTree(const nlohmann::json& behaviorJson, const std::string& monsterId)
{
	// For the root node, which is always a MultiConfig
	std::string rootType = behaviorJson.value("type", "MultiConfig");

	// If the root is a MultiConfig, we'll handle it specially
	if (rootType == "MultiConfig") {
		auto rootBehavior = std::make_unique<BehaviorMultiConfig>();

		// Set container type based on behaviorType field
		if (behaviorJson.contains("behaviorType")) {
			std::string behaviorType = behaviorJson["behaviorType"];
			rootBehavior->containerType = StringToContainerType(behaviorType);
		}

		// Process child behaviors if present
		if (behaviorJson.contains("behaviors") && behaviorJson["behaviors"].is_array()) {
			for (const auto& childJson : behaviorJson["behaviors"]) {
				if (childJson.contains("type")) {
					std::string type = childJson["type"];

					if (type == "MultiConfig") {
						// Recursively process nested MultiConfig
						auto nestedMulti = ImportBehaviorTree(childJson, monsterId);
						rootBehavior->childBehaviors.push_back(std::move(nestedMulti));
					}
					else {
						// Use the behavior registry to import other behavior types
						auto importer = BehaviorRegistry::Instance().GetImporter(type);
						if (importer) {
							auto childBehavior = importer(childJson, monsterId, m_importedBulletConfigs);
							if (childBehavior) {
								rootBehavior->childBehaviors.push_back(std::move(childBehavior));
							}
							else {
								LOG_ERROR("Failed to import behavior of type: " + type);
							}
						}
						else {
							LOG_ERROR("No importer found for behavior type: " + type);
						}
					}
				}
			}
		}

		return rootBehavior;
	}
	else {
		// If the root is not a MultiConfig (unusual case), create a MultiConfig 
		// and use the registry to import the root as a child
		auto rootBehavior = std::make_unique<BehaviorMultiConfig>();
		auto importer = BehaviorRegistry::Instance().GetImporter(rootType);

		if (importer) {
			auto behavior = importer(behaviorJson, monsterId, m_importedBulletConfigs);
			if (behavior) {
				rootBehavior->childBehaviors.push_back(std::move(behavior));
			}
		}
		else {
			LOG_ERROR("No importer found for root behavior type: " + rootType);
		}

		return rootBehavior;
	}
}

