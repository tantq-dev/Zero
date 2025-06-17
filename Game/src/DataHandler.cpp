#include "DataHandler.h"
#include "BulletModel.h"
#include "utilities/Logger.h"
#include "MonsterModel.h"
#include <memory>
#include <fstream>
#include <unordered_set>
#include "core/EventSystem.h"
#include "EventKey.h"
#include <WaveModel.h>

void Tool::DataHandler::ExportBulletConfig(const std::vector<MonsterTypeDefinition*>& monsterDefinitions)
{
	LOG_INFO("Exporting bullet config for monsters" + std::to_string(monsterDefinitions.size()));
	int id = 0;
	for (auto monsterDef : monsterDefinitions)
	{
		if (monsterDef) {
			GetBulletPropertiesFromMultipleBehavior(monsterDef->defaultProperties.rootBehavior.get());
			ExportToJson();
		}
	}
	ExportMonsterToJson(monsterDefinitions);

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
			m_bullets.push_back(castedBehavior->bulletConfig);
		}
		else if (auto castedBehavior = dynamic_cast<BehaviorShootProjectileConfig*>(behavior.get()))
		{
			m_bullets.push_back(castedBehavior->bulletConfig);

		}
		else if (auto castedBehavior = dynamic_cast<BehaviorSpreadShotConfig*>(behavior.get()))
		{
			m_bullets.push_back(castedBehavior->bulletConfig);

		}
	}
}

void Tool::DataHandler::ExportToJson()
{
	// Create the root JSON object with bullets container
	nlohmann::json rootJson;
	nlohmann::json bulletsJson;

	// Process each bullet in the collection
	int uniqueCounter = 1;
	std::unordered_set<std::string> usedIDs;

	for (const auto& bullet : m_bullets)
	{
		std::string bulletID = GetBulletID(bullet);

		// Skip duplication bullet
		if (usedIDs.find(bulletID) != usedIDs.end()) {
			return;
		}
		usedIDs.insert(bulletID);

		// Create bullet JSON object with all required fields
		nlohmann::json bulletJson;
		bulletJson["ID"] = bulletID;
		bulletJson["AssetID"] = "9952908011400548"; // Default asset ID
		bulletJson["MoveSpeed"] = bullet.speed;
		bulletJson["Damage"] = bullet.damage;
		bulletJson["AliveTime"] = bullet.aliveTime;
		bulletJson["Elemental"] = ""; // Not specified in bullet config
		bulletJson["Bounce"] = bullet.bounce;

		// Map bullet type to MoveBehavior type
		nlohmann::json moveBehavior;
		bulletJson["Type"] = BulletTypeToString(bullet.bulletType);
		bulletJson["MoveBehavior"] = moveBehavior;

		// Add this bullet to the bullets collection
		bulletsJson[bulletID] = bulletJson;
	}

	rootJson["bullets"] = bulletsJson;

	// Write to file
	std::ofstream outFile("bullets.json");
	if (outFile.is_open())
	{
		outFile << rootJson.dump(2); // Pretty print with 2-space indentation
		outFile.close();
		LOG_INFO("Successfully exported " + std::to_string(m_bullets.size()) + " bullets to bullets.json");
	}
	else
	{
		LOG_ERROR("Failed to open bullets.json for writing");
	}
}

// Add this function to export monster data
void Tool::DataHandler::ExportMonsterToJson(const std::vector<MonsterTypeDefinition*>& monsterDefinitions)
{
	LOG_INFO("Exporting monster config for " + std::to_string(monsterDefinitions.size()) + " monsters");

	nlohmann::json rootJson;
	nlohmann::json enemiesJson;

	int uniqueCounter = 1;
	std::unordered_set<std::string> usedIDs;

	for (auto monsterDef : monsterDefinitions)
	{
		if (!monsterDef) continue;

		// Generate a unique ID for this monster
		std::string monsterId = "enemy_" + std::to_string(monsterDef->item.id);
		usedIDs.insert(monsterId);

		nlohmann::json monsterJson;
		monsterJson["ID"] = monsterId;
		monsterJson["Name"] = monsterDef->defaultProperties.name;


		//todo: not use asset id use monster type instead ingame should modify
		// Use texture name as asset ID or generate a placeholder
		//std::string assetId = "1000000000000000";
		//if (MonsterTextureMap.find(monsterDef->defaultProperties.valideMonsterIngame) != MonsterTextureMap.end()) {
		//	// In a real implementation, you'd map texture paths to proper asset IDs
		//	// Here we're just creating a placeholder based on the monster name
		//	assetId = std::to_string(std::hash<std::string>{}(monsterDef->defaultProperties.valideMonsterIngame) % 10000000000000000);
		//}
		//! just let feild = "AssetID" for now 
		monsterJson["AssetID"] = monsterDef->defaultProperties.valideMonsterIngame;

		// Basic stats
		monsterJson["MaxHealth"] = monsterDef->defaultProperties.hp;
		// Convert knockbackResistance from integer to float (0.0-1.0)
		float knockbackResistance = monsterDef->defaultProperties.knockbackResistance / 10000.0f;
		monsterJson["KnockBackResistance"] = knockbackResistance;
		// Convert speed from integer to float
		float moveSpeed = monsterDef->defaultProperties.speed / 10000.0f;
		monsterJson["MoveSpeed"] = moveSpeed;
		// Monster type
		monsterJson["EnemyType"] = (monsterDef->defaultProperties.monsterType == MonsterType::Boss) ? "BOSS" : "NORMAL";
		monsterJson["CollisionDamage"] = monsterDef->defaultProperties.collisionDamage;
		// Default values for fields not in our model
		monsterJson["Defense"] = 0;
		monsterJson["DodgeRate"] = 0;

		// Process behavior tree
		if (monsterDef->defaultProperties.rootBehavior) {
			monsterJson["BehaviorConfig"] = ProcessBehaviorTree(monsterDef->defaultProperties.rootBehavior.get(), monsterId);
		}

		// Add to monsters collection
		enemiesJson[monsterId] = monsterJson;
	}

	rootJson["enemies"] = enemiesJson;

	// Write to file
	std::ofstream outFile("monsters.json");
	if (outFile.is_open()) {
		outFile << rootJson.dump(2); // Pretty print with 2-space indentation
		outFile.close();
		LOG_INFO("Successfully exported " + std::to_string(monsterDefinitions.size()) + " monsters to monsters.json");
	}
	else {
		LOG_ERROR("Failed to open monsters.json for writing");
	}
}

void Tool::DataHandler::ExportPositionJson(std::vector<MonsterWave*> m_pWaveInformations, entt::registry& registry)
{
	LOG_INFO("Exporting monster wave positions to JSON");

	nlohmann::json rootJson;
	nlohmann::json wavesJson = nlohmann::json::array();

	// Process each wave
	for (auto wavePtr : m_pWaveInformations)
	{
		if (!wavePtr) continue;

		nlohmann::json waveJson;
		waveJson["waveIndex"] = wavePtr->waveIndex;
		waveJson["isBossWave"] = wavePtr->isBossWave;

		// Create array of monsters in this wave
		nlohmann::json monstersJson = nlohmann::json::array();

		for (auto entityId : wavePtr->monsterEntities)
		{
			if (!registry.valid(entityId)) continue;

			auto* placedMonster = registry.try_get<PlacedMonster>(entityId);
			if (!placedMonster) continue;

			nlohmann::json monsterJson;

			// Get monster ID from the imported monster data or create based on name
			std::string monsterId = "enemy_" + std::to_string(placedMonster->item.id);

			monsterJson["monsterId"] = monsterId;
			monsterJson["gridPosition"] = {
				{"x", placedMonster->positionInGrid.x},
				{"y", placedMonster->positionInGrid.y}
			};

			monstersJson.push_back(monsterJson);
		}

		waveJson["monsters"] = monstersJson;
		wavesJson.push_back(waveJson);
	}

	rootJson["waves"] = wavesJson;

	// Write to file
	std::ofstream outFile("monster_waves.json");
	if (outFile.is_open())
	{
		outFile << rootJson.dump(2); // Pretty print with 2-space indentation
		outFile.close();
		LOG_INFO("Successfully exported " + std::to_string(m_pWaveInformations.size()) + " waves to monster_waves.json");
	}
	else
	{
		LOG_ERROR("Failed to open monster_waves.json for writing");
	}
}



// Helper function to recursively process behavior tree
nlohmann::json Tool::DataHandler::ProcessBehaviorTree(const BehaviorConfig* behavior, const std::string& monsterId)
{
	nlohmann::json behaviorJson;

	if (auto multiConfig = dynamic_cast<const BehaviorMultiConfig*>(behavior)) {
		behaviorJson["type"] = "MultiConfig";

		// Map container type to behaviorType string
		switch (multiConfig->containerType) {
		case ContainerType::SelectorWithRunning:
			behaviorJson["behaviorType"] = "Selector";
			break;
		case ContainerType::ProgressiveSequence:
			behaviorJson["behaviorType"] = "ProgressiveSequence";
			break;
		case ContainerType::Sequence:
			behaviorJson["behaviorType"] = "Sequence";
			break;
		}

		// Process child behaviors
		nlohmann::json behaviorsArray = nlohmann::json::array();
		for (const auto& childBehavior : multiConfig->childBehaviors) {
			if (childBehavior) {
				behaviorsArray.push_back(ProcessBehaviorTree(childBehavior.get(), monsterId));
			}
		}
		behaviorJson["behaviors"] = behaviorsArray;
	}
	else if (auto chaseConfig = dynamic_cast<const BehaviorChaseConfig*>(behavior)) {
		behaviorJson["type"] = "Chase";
		behaviorJson["speed"] = chaseConfig->chaseSpeed / 10000.0f; // Convert to float
	}
	else if (auto distanceConfig = dynamic_cast<const BehaviorDistanceConditionHelperConfig*>(behavior)) {
		behaviorJson["type"] = "DistanceConditionHelper";
		behaviorJson["minDistance"] = distanceConfig->minDistance / 10000.0f; // Convert to float
		behaviorJson["maxDistance"] = distanceConfig->maxDistance / 10000.0f; // Convert to float
	}
	else if (auto bounceConfig = dynamic_cast<const BehaviorMovementBounceConfig*>(behavior)) {
		behaviorJson["type"] = "MovementBounce";
	}
	else if (auto projectileConfig = dynamic_cast<const BehaviorShootProjectileConfig*>(behavior)) {
		behaviorJson["type"] = "ShootProjectile";
		behaviorJson["cooldown"] = projectileConfig->coolDown / 10000.0f; // Convert to float

		// Bullet ID from bullet config or generate a placeholder
		std::string bulletId = GetBulletID(projectileConfig->bulletConfig);
		if (bulletId.empty()) {
			bulletId = "bullet_straight_01"; // Default bullet ID
		}
		behaviorJson["bulletID"] = bulletId;
		behaviorJson["enemyID"] = monsterId;
	}
	else if (auto spreadConfig = dynamic_cast<const BehaviorSpreadShotConfig*>(behavior)) {
		behaviorJson["type"] = "SpreadShot";
		behaviorJson["cooldown"] = spreadConfig->coolDown / 10000.0f; // Convert to float

		// Bullet ID from bullet config or generate a placeholder
		std::string bulletId = GetBulletID(spreadConfig->bulletConfig);
		if (bulletId.empty()) {
			bulletId = "bullet_straight_01"; // Default bullet ID
		}
		behaviorJson["bulletID"] = bulletId;
		behaviorJson["enemyID"] = monsterId;
		behaviorJson["bulletCount"] = spreadConfig->numOfBullet;
		behaviorJson["spreadAngle"] = spreadConfig->spreadAngle;
	}
	else if (auto barrageConfig = dynamic_cast<const BehaviorShootBarrageConfig*>(behavior)) {
		behaviorJson["type"] = "ShootBarrage";
		behaviorJson["cooldown"] = barrageConfig->coolDown / 10000.0f; // Convert to float

		// Bullet ID from bullet config or generate a placeholder
		std::string bulletId = GetBulletID(barrageConfig->bulletConfig);
		if (bulletId.empty()) {
			bulletId = "bullet_straight_01"; // Default bullet ID
		}
		behaviorJson["bulletID"] = bulletId;
		behaviorJson["enemyID"] = monsterId;
		behaviorJson["bulletCount"] = barrageConfig->numOfBullet;
		behaviorJson["spreadAngle"] = barrageConfig->spreadAngle;
	}
	else {
		behaviorJson["type"] = "Unknown";
	}

	return behaviorJson;
}

std::string Tool::DataHandler::GetBulletID(const BulletConfig& bulletConfig)
{

	// If no ID specified, generate one based on the bullet properties
	// This ensures the same bullet config gets the same ID across exports
	std::string properties =
		std::to_string(static_cast<int>(bulletConfig.bulletType)) + "_" +
		std::to_string(bulletConfig.speed) + "_" +
		std::to_string(bulletConfig.damage) + "_" +
		std::to_string(bulletConfig.aliveTime) + "_" +
		std::to_string(bulletConfig.bounce);

	// Generate a hash of the properties to create a consistent ID
	size_t hash = std::hash<std::string>{}(properties);

	// Create a bullet ID format that matches the example
	return "bullet_" + BulletTypeToString(bulletConfig.bulletType) + "_" + std::to_string(hash % 1000);
}

std::string Tool::DataHandler::BulletTypeToString(BulletType type) {
	switch (type)
	{
	case BulletType::Straight:
		return "Bullet_Straight";
		break;
	case BulletType::Parabol:
		return "Bullet_Parabol";
		break;
	case BulletType::Mortal:
		return "Bullet_Mortar";
		break;
	case BulletType::Boss:
		return "Bullet_Boss";
		break;
	default:
		return "Bullet_Straight"; // bullet straight as default
	}
}

void Tool::DataHandler::ImportMonsterFromJson()
{
	// Clear any previously imported monster definitions
	m_importedMonsterDefinitions.clear();

	LOG_INFO("Importing monsters from JSON");

	std::ifstream inFile("monsters.json");
	if (!inFile.is_open()) {
		LOG_ERROR("Failed to open monsters.json for reading");
		return;
	}

	try {
		// Parse the JSON file
		nlohmann::json rootJson;
		inFile >> rootJson;
		inFile.close();

		// Check if 'enemies' key exists
		if (!rootJson.contains("enemies")) {
			LOG_ERROR("JSON file does not contain 'enemies' section");
			return;
		}

		const auto& enemiesJson = rootJson["enemies"];

		// Process each enemy in the JSON
		for (auto it = enemiesJson.begin(); it != enemiesJson.end(); ++it) {
			const auto& enemyJson = it.value();

			// Create a new monster definition
			MonsterTypeDefinition monsterDef;
			std::string id = enemyJson.value("ID", "enemy_0");
			// Set basic properties
			monsterDef.item.id = std::stoi(id.substr(6));; // Generate ID based on position
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
			monsterDef.defaultProperties.knockbackResistance = static_cast<int>(knockback * 10000.0f);

			float speed = enemyJson.value("MoveSpeed", 1.5f);
			monsterDef.defaultProperties.speed = static_cast<int>(speed * 10000.0f);

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
		LOG_ERROR("Error parsing monsters.json: " + std::string(e.what()));
	}
}

void Tool::DataHandler::ImportBulletFromJson()
{
	// Clear any previously imported bullet configs
	m_importedBulletConfigs.clear();

	LOG_INFO("Importing bullets from JSON");

	std::ifstream inFile("bullets.json");
	if (!inFile.is_open()) {
		LOG_ERROR("Failed to open bullets.json for reading");
		return;
	}

	try {
		// Parse the JSON file
		nlohmann::json rootJson;
		inFile >> rootJson;
		inFile.close();

		// Check if 'bullets' key exists
		if (!rootJson.contains("bullets")) {
			LOG_ERROR("JSON file does not contain 'bullets' section");
			return;
		}

		const auto& bulletsJson = rootJson["bullets"];

		// Process each bullet in the JSON
		for (auto it = bulletsJson.begin(); it != bulletsJson.end(); ++it) {
			const std::string& bulletId = it.key();
			const auto& bulletJson = it.value();

			// Create a new bullet config
			BulletConfig bulletConfig;

			// Set the valid bullet ingame identifier
			bulletConfig.validBulletIngame = bulletId;

			// Set properties from JSON
			bulletConfig.speed = bulletJson.value("MoveSpeed", 0);
			bulletConfig.damage = bulletJson.value("Damage", 0);
			bulletConfig.aliveTime = bulletJson.value("AliveTime", 0);
			bulletConfig.bounce = bulletJson.value("Bounce", 0);

			// Determine bullet type from MoveBehavior
			std::string moveBehaviorType = "";
			if (bulletJson.contains("MoveBehavior") && bulletJson["MoveBehavior"].contains("type")) {
				moveBehaviorType = bulletJson["MoveBehavior"]["type"];
			}
			else if (bulletJson.contains("Type")) {
				moveBehaviorType = bulletJson["Type"];
			}

			// Set bullet type based on move behavior
			if (moveBehaviorType == "Bullet_Straight") {
				bulletConfig.bulletType = BulletType::Straight;
			}
			else if (moveBehaviorType == "Bullet_Parabol") {
				bulletConfig.bulletType = BulletType::Parabol;
			}
			else if (moveBehaviorType == "Bullet_Mortar") {
				bulletConfig.bulletType = BulletType::Mortal;
			}
			else if (moveBehaviorType == "Bullet_Boss") {
				bulletConfig.bulletType = BulletType::Boss;
			}
			else {
				bulletConfig.bulletType = BulletType::Straight; // Default
			}

			// Add to imported bullet configs
			m_importedBulletConfigs[bulletId] = bulletConfig;
		}

		LOG_INFO("Successfully imported " + std::to_string(m_importedBulletConfigs.size()) + " bullets");
	}
	catch (const std::exception& e) {
		LOG_ERROR("Error parsing bullets.json: " + std::string(e.what()));
	}
}

void Tool::DataHandler::ImportMonsterWavesFromJson(System::GridSystem* gridSystem)
{
	// Clear any previously imported wave data
	m_importedMonsterWave.clear();

	LOG_INFO("Importing monster waves from JSON");

	std::ifstream inFile("monster_waves.json");
	if (!inFile.is_open()) {
		LOG_ERROR("Failed to open monster_waves.json for reading");
		return;
	}

	try {
		// Parse the JSON file
		nlohmann::json rootJson;
		inFile >> rootJson;
		inFile.close();

		// Check if 'waves' key exists
		if (!rootJson.contains("waves")) {
			LOG_ERROR("JSON file does not contain 'waves' section");
			return;
		}

		const auto& wavesJson = rootJson["waves"];

		// First, ensure we have our monsters loaded
		if (m_importedMonsterDefinitions.empty()) {
			ImportMonsterFromJson();
		}

		// Process each wave in the JSON
		for (const auto& waveJson : wavesJson) {
			MonsterWave wave;

			// Set wave properties
			wave.waveIndex = waveJson.value("waveIndex", 0);
			wave.isBossWave = waveJson.value("isBossWave", false);

			// We'll populate the monsterEntities later when sending to MapEditor
			// For now, we just store the information needed to create the entities

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

					// Find the matching monster definition by extracting numeric ID from "enemy_XX" format
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
						LOG_INFO("Imported monster: " + def.textureName);
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
		LOG_ERROR("Error parsing monster_waves.json: " + std::string(e.what()));
	}
}

void Tool::DataHandler::ImportAllData(System::GridSystem* gridSystem)
{
	ImportBulletFromJson();
	ImportMonsterFromJson();
	ImportMonsterWavesFromJson(gridSystem);
	LOG_INFO("All data imported successfully");
	Core::EventData evData;
	evData.data = m_importedMonsterDefinitions;
	Core::EventSystem::getInstance().publish(EventKeys::SendMonsterData, evData);
}

// Helper function to recursively import behavior tree from JSON
std::unique_ptr<BehaviorMultiConfig> Tool::DataHandler::ImportBehaviorTree(const nlohmann::json& behaviorJson, const std::string& monsterId)
{
	auto rootBehavior = std::make_unique<BehaviorMultiConfig>();

	// Set container type based on behaviorType field
	if (behaviorJson.contains("behaviorType")) {
		std::string behaviorType = behaviorJson["behaviorType"];
		if (behaviorType == "Selector") {
			rootBehavior->containerType = ContainerType::SelectorWithRunning;
		}
		else if (behaviorType == "ProgressiveSequence") {
			rootBehavior->containerType = ContainerType::ProgressiveSequence;
		}
		else if (behaviorType == "Sequence") {
			rootBehavior->containerType = ContainerType::Sequence;
		}
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
				else if (type == "Chase") {
					auto chaseConfig = std::make_unique<BehaviorChaseConfig>();
					if (childJson.contains("speed")) {
						float speed = childJson["speed"];
						chaseConfig->chaseSpeed = static_cast<int>(speed * 10000.0f);
					}
					rootBehavior->childBehaviors.push_back(std::move(chaseConfig));
				}
				else if (type == "DistanceConditionHelper") {
					auto distanceConfig = std::make_unique<BehaviorDistanceConditionHelperConfig>();
					if (childJson.contains("minDistance")) {
						float minDist = childJson["minDistance"];
						distanceConfig->minDistance = static_cast<int>(minDist * 10000.0f);
					}
					if (childJson.contains("maxDistance")) {
						float maxDist = childJson["maxDistance"];
						distanceConfig->maxDistance = static_cast<int>(maxDist * 10000.0f);
					}
					rootBehavior->childBehaviors.push_back(std::move(distanceConfig));
				}
				else if (type == "MovementBounce") {
					auto bounceConfig = std::make_unique<BehaviorMovementBounceConfig>();
					rootBehavior->childBehaviors.push_back(std::move(bounceConfig));
				}
				else if (type == "ShootProjectile") {
					auto projectileConfig = std::make_unique<BehaviorShootProjectileConfig>();
					if (childJson.contains("cooldown")) {
						float cooldown = childJson["cooldown"];
						projectileConfig->coolDown = static_cast<int>(cooldown * 10000.0f);
					}

					// Set bullet config if a matching bullet ID exists
					if (childJson.contains("bulletID")) {
						std::string bulletId = childJson["bulletID"];
						if (m_importedBulletConfigs.find(bulletId) != m_importedBulletConfigs.end()) {
							projectileConfig->bulletConfig = m_importedBulletConfigs[bulletId];
						}
					}

					rootBehavior->childBehaviors.push_back(std::move(projectileConfig));
				}
				else if (type == "SpreadShot") {
					auto spreadConfig = std::make_unique<BehaviorSpreadShotConfig>();
					if (childJson.contains("cooldown")) {
						float cooldown = childJson["cooldown"];
						spreadConfig->coolDown = static_cast<int>(cooldown * 10000.0f);
					}
					if (childJson.contains("bulletCount")) {
						spreadConfig->numOfBullet = childJson["bulletCount"];
					}
					if (childJson.contains("spreadAngle")) {
						spreadConfig->spreadAngle = childJson["spreadAngle"];
					}

					// Set bullet config if a matching bullet ID exists
					if (childJson.contains("bulletID")) {
						std::string bulletId = childJson["bulletID"];
						if (m_importedBulletConfigs.find(bulletId) != m_importedBulletConfigs.end()) {
							spreadConfig->bulletConfig = m_importedBulletConfigs[bulletId];
						}
					}

					rootBehavior->childBehaviors.push_back(std::move(spreadConfig));
				}
				else if (type == "ShootBarrage") {
					auto barrageConfig = std::make_unique<BehaviorShootBarrageConfig>();
					if (childJson.contains("cooldown")) {
						float cooldown = childJson["cooldown"];
						barrageConfig->coolDown = static_cast<int>(cooldown * 10000.0f);
					}
					if (childJson.contains("bulletCount")) {
						barrageConfig->numOfBullet = childJson["bulletCount"];
					}
					if (childJson.contains("spreadAngle")) {
						barrageConfig->spreadAngle = childJson["spreadAngle"];
					}

					// Set bullet config if a matching bullet ID exists
					if (childJson.contains("bulletID")) {
						std::string bulletId = childJson["bulletID"];
						if (m_importedBulletConfigs.find(bulletId) != m_importedBulletConfigs.end()) {
							barrageConfig->bulletConfig = m_importedBulletConfigs[bulletId];
						}
					}

					rootBehavior->childBehaviors.push_back(std::move(barrageConfig));
				}
			}
		}
	}

	return rootBehavior;
}