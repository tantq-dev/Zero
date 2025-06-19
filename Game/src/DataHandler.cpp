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


void Tool::DataHandler::ExportAllToSingleJson(const std::vector<MonsterTypeDefinition*>& monsterDefinitions, std::vector<MonsterWave*> waveInformations, entt::registry& registry, const std::string& mapId, const std::string& mapName)
{
	m_registry = &registry; // Store the registry pointer
	LOG_INFO("Exporting all data to JSON file");

	nlohmann::json rootJson;

	// Process bullets
	GetBulletsFromMonsters(monsterDefinitions);

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

	// Bullets section (from ExportToJson)
	nlohmann::json bulletsJson;
	std::unordered_set<std::string> usedIDs;

	for (const auto& bullet : m_bullets)
	{
		std::string bulletID = GetBulletID(bullet);

		// Skip duplicate bullets
		if (usedIDs.find(bulletID) != usedIDs.end()) {
			continue;
		}
		usedIDs.insert(bulletID);

		nlohmann::json bulletJson;
		bulletJson["ID"] = bulletID;
		bulletJson["AssetID"] = bullet.validBulletIngame == std::string() ? "bullet_01" : bullet.validBulletIngame; // Default asset ID
		bulletJson["MoveSpeed"] = bullet.speed;
		bulletJson["Damage"] = bullet.damage;
		bulletJson["AliveTime"] = bullet.aliveTime;
		bulletJson["Elemental"] = "";
		bulletJson["Bounce"] = bullet.bounce;

		nlohmann::json moveBehavior;
		moveBehavior["type"] = BulletTypeToString(bullet.bulletType);
		bulletJson["MoveBehavior"] = moveBehavior;

		bulletsJson[bulletID] = bulletJson;
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
	m_bullets.clear();
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

	LOG_INFO("Importing bullets");

	try {
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
			monsterDef.defaultProperties.knockbackResistance = static_cast<int>(knockback);

			float speed = enemyJson.value("MoveSpeed", 1.5f);
			monsterDef.defaultProperties.speed = static_cast<int>(speed);

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
	nlohmann::json behaviorJson;

	if (auto multiConfig = dynamic_cast<const BehaviorMultiConfig*>(behavior)) {
		behaviorJson["type"] = "MultiConfig";
		// Map container type to behaviorType string
		switch (multiConfig->containerType) {
		case ContainerType::SelectorWithRunning:
			behaviorJson["behaviorType"] = "SelectorWithRunning";
			break;
		case ContainerType::ProgressiveSequence:
			behaviorJson["behaviorType"] = "ProgressiveSequence";
			break;
		case ContainerType::Sequence:
			behaviorJson["behaviorType"] = "Sequence";
			break;
		case ContainerType::Selector:
			behaviorJson["behaviorType"] = "Selector";
			break;
		case ContainerType::Race:
			behaviorJson["behaviorType"] = "Race";
			break;
		case ContainerType::Parallel:
			behaviorJson["behaviorType"] = "Parallel";
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
		behaviorJson["speed"] = chaseConfig->chaseSpeed; // Convert to float
	}
	else if (auto distanceConfig = dynamic_cast<const BehaviorDistanceConditionHelperConfig*>(behavior)) {
		behaviorJson["type"] = "DistanceConditionHelper";
		behaviorJson["minDistance"] = distanceConfig->minDistance; // Convert to float
		behaviorJson["maxDistance"] = distanceConfig->maxDistance; // Convert to float
	}
	else if (auto bounceConfig = dynamic_cast<const BehaviorMovementBounceConfig*>(behavior)) {
		behaviorJson["type"] = "MovementBounce";
	}
	else if (auto projectileConfig = dynamic_cast<const BehaviorShootProjectileConfig*>(behavior)) {
		behaviorJson["type"] = "ShootProjectile";
		behaviorJson["cooldown"] = projectileConfig->coolDown; // Convert to float

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
		behaviorJson["cooldown"] = spreadConfig->coolDown; // Convert to float

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
		behaviorJson["cooldown"] = barrageConfig->coolDown; // Convert to float

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



void Tool::DataHandler::ImportAllData(System::GridSystem* gridSystem)
{
	ImportFromSingleJson(gridSystem);

}

// Helper function to recursively import behavior tree from JSON
std::unique_ptr<BehaviorMultiConfig> Tool::DataHandler::ImportBehaviorTree(const nlohmann::json& behaviorJson, const std::string& monsterId)
{
	auto rootBehavior = std::make_unique<BehaviorMultiConfig>();

	// Set container type based on behaviorType field
	if (behaviorJson.contains("behaviorType")) {
		std::string behaviorType = behaviorJson["behaviorType"];
		if (behaviorType == "SelectorWithRunning") {
			rootBehavior->containerType = ContainerType::SelectorWithRunning;
		}
		else if (behaviorType == "ProgressiveSequence") {
			rootBehavior->containerType = ContainerType::ProgressiveSequence;
		}
		else if (behaviorType == "Sequence") {
			rootBehavior->containerType = ContainerType::Sequence;
		}
		else if (behaviorType == "Selector") {
			rootBehavior->containerType = ContainerType::Selector;
		}
		else if (behaviorType == "Parallel") {
			rootBehavior->containerType = ContainerType::Parallel;
		}
		else if (behaviorType == "Race") {
			rootBehavior->containerType = ContainerType::Race;
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
						chaseConfig->chaseSpeed = static_cast<int>(speed);
					}
					rootBehavior->childBehaviors.push_back(std::move(chaseConfig));
				}
				else if (type == "DistanceConditionHelper") {
					auto distanceConfig = std::make_unique<BehaviorDistanceConditionHelperConfig>();
					if (childJson.contains("minDistance")) {
						float minDist = childJson["minDistance"];
						distanceConfig->minDistance = static_cast<int>(minDist);
					}
					if (childJson.contains("maxDistance")) {
						float maxDist = childJson["maxDistance"];
						distanceConfig->maxDistance = static_cast<int>(maxDist);
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
						projectileConfig->coolDown = static_cast<int>(cooldown);
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
						spreadConfig->coolDown = static_cast<int>(cooldown);
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
						barrageConfig->coolDown = static_cast<int>(cooldown);
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