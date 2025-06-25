#pragma once  
#include <string>  
#include <unordered_map>  
#include <vector>
#include <memory>
#include "utilities/Vec2.h"
#include <functional>
#include <json.hpp>


// Add this enum at an appropriate location in the file
enum class BehaviorType {
	Chase,
	DistanceConditionHelper,
	MovementBounce,
	ShootBarrage,
	ShootProjectile,
	ShootStrategyBase,
	SpreadShot,
	MultiConfig,
	COUNT // to count member of enum; please add member above this count
};


inline std::string BehaviorTypeToString(BehaviorType type) {
	switch (type) {
	case BehaviorType::Chase: return "Chase";
	case BehaviorType::DistanceConditionHelper: return "DistanceConditionHelper";
	case BehaviorType::MovementBounce: return "MovementBounce";
	case BehaviorType::ShootBarrage: return "ShootBarrage";
	case BehaviorType::ShootProjectile: return "ShootProjectile";
	case BehaviorType::ShootStrategyBase: return "ShootStrategyBase";
	case BehaviorType::SpreadShot: return "SpreadShot";
	case BehaviorType::MultiConfig: return "MultiConfig";
	default: return "Unknown";
	}
}


inline BehaviorType StringToBehaviorType(const std::string& typeStr) {
	if (typeStr == "Chase") return BehaviorType::Chase;
	if (typeStr == "DistanceConditionHelper") return BehaviorType::DistanceConditionHelper;
	if (typeStr == "MovementBounce") return BehaviorType::MovementBounce;
	if (typeStr == "ShootBarrage") return BehaviorType::ShootBarrage;
	if (typeStr == "ShootProjectile") return BehaviorType::ShootProjectile;
	if (typeStr == "ShootStrategyBase") return BehaviorType::ShootStrategyBase;
	if (typeStr == "SpreadShot") return BehaviorType::SpreadShot;
	if (typeStr == "MultiConfig") return BehaviorType::MultiConfig;
	return BehaviorType::Chase; // Default
}

inline std::vector<BehaviorType> GetAvailableBehaviorTypes() {
	std::vector<BehaviorType> behaviors;
	for (int i = 0; i < static_cast<int>(BehaviorType::COUNT); ++i) {
		behaviors.push_back(static_cast<BehaviorType>(i));
	}
	return behaviors;
}


enum class ConfigFieldType {
	Int,
	Combo,
	Text,
	Bullet,  // Special field type for bullet configuration
	BulletID, // Special field type for bullet ID selection
	None
};

enum class BulletType {
	Straight,
	Parabol,
	Mortal,
	Boss,
	COUNT // to count member of enum; please add member above this count
};

inline std::string BulletTypeToString(BulletType type) {
	switch (type) {
	case BulletType::Straight:
		return "Straight";
	case BulletType::Mortal:
		return "Mortal";
	case BulletType::Parabol:
		return "Parabol";
	case BulletType::Boss:
		return "Boss";
	default:
		return "Unknow";
	}
}

inline BulletType StringToBulletType(const std::string& str) {
	if (str == "Straight") return BulletType::Straight;
	if (str == "Parabol") return BulletType::Parabol;
	if (str == "Mortal")  return BulletType::Mortal;
	if (str == "Boss")    return BulletType::Boss;
	return BulletType::COUNT;
}

inline std::vector<std::string> GetBulletsTypeString() {
	std::vector<std::string> bulletString;
	for (int i = 0; i < static_cast<int>(BulletType::COUNT); ++i) {
		bulletString.push_back(BulletTypeToString(static_cast<BulletType>(i)));
	}
	return bulletString;
}

static std::unordered_map<std::string, const char*> BulletTextureMap = {
	{"bullet_01", ""},
};

struct BulletConfig {
	BulletType bulletType = BulletType::Parabol;
	std::string validBulletIngame = BulletTextureMap.begin()->first;
	int speed = 0;
	int aliveTime = 0;
	int damage = 0;
	int bounce = 0;

	// Copy constructor and assignment operator for BulletConfig
	BulletConfig() = default;
	BulletConfig(const BulletConfig&) = default;
	BulletConfig& operator=(const BulletConfig&) = default;
	static std::string GetBulletID(const BulletConfig& bulletConfig);

};


struct ConfigField {
	ConfigFieldType type;
	std::string name;
	void* valuePtr;
	std::vector<std::string> options;  // For combo boxes
	std::function<bool(void*)> onChange;  // Optional callback when value changes

	// Constructor for Int fields
	ConfigField(const std::string& fieldName, int* ptr,
		std::function<bool(void*)> callback = nullptr)
		: type(ConfigFieldType::Int), name(fieldName), valuePtr(ptr), onChange(callback) {
	}

	// Constructor for Combo fields
	ConfigField(const std::string& fieldName, int* ptr,
		const std::vector<std::string>& comboOptions,
		std::function<bool(void*)> callback = nullptr)
		: type(ConfigFieldType::Combo), name(fieldName), valuePtr(ptr),
		options(comboOptions), onChange(callback) {
	}

	// Constructor for Text fields
	ConfigField(const std::string& fieldName, std::string* ptr,
		std::function<bool(void*)> callback = nullptr)
		: type(ConfigFieldType::Text), name(fieldName), valuePtr(ptr), onChange(callback) {
	}

	// Constructor for Bullet fields (special case)
	ConfigField(const std::string& fieldName, BulletConfig* ptr,
		std::function<bool(void*)> callback = nullptr)
		: type(ConfigFieldType::Bullet), name(fieldName), valuePtr(ptr), onChange(callback) {
	}

	// Constructor for BulletID fields (dropdown selection)
	ConfigField(const std::string& fieldName, std::string* ptr, ConfigFieldType fieldType,
		std::function<bool(void*)> callback = nullptr)
		: type(fieldType), name(fieldName), valuePtr(ptr), onChange(callback) {
		// Only allow BulletID type for this constructor
		if (fieldType != ConfigFieldType::BulletID) {
			type = ConfigFieldType::Text;
		}
	}
};



// Define the static member variable outside the struct  
static std::unordered_map<std::string, const char*> MonsterTextureMap = {
	{"Slime",			"assets//Slug.bmp"},
	{"Ranger_Slime",	"assets//Starflake.bmp"},
	{"Android",			"assets//Snowman.bmp" },
	{"Skeleton",		"assets/Spicatus.bmp"},
	{"Boss",			"assets//Thorn dragon.bmp"}
};

struct MonsterItem {
	int id;
	std::string name;
};

// Enums for various property types
enum class MonsterType {
	Normal,
	Boss,
	COUNT
};

inline std::string MonsterTypeToString(MonsterType monsterType) {
	switch (monsterType) {
	case MonsterType::Normal:
		return "Normal";
	case MonsterType::Boss:
		return "Boss";
	default:
		return "Unknow";
	}
}

inline std::vector<std::string> GetMonstersTypeString() {
	std::vector<std::string> monsterTypeString;
	for (int i = 0; i < static_cast<int>(MonsterType::COUNT); ++i) {
		monsterTypeString.push_back(MonsterTypeToString(static_cast<MonsterType>(i)));
	}
	return monsterTypeString;
}



enum class ContainerType {
	SelectorWithRunning,
	ProgressiveSequence,
	Sequence,
	Selector,
	Parallel,
	Race,
	COUNT //use to count enum member please add new member above it 
};

inline std::string ContainerTypeToString(const ContainerType& type) {
	switch (type) {
	case ContainerType::SelectorWithRunning:
		return "SelectorWithRunning";
	case ContainerType::ProgressiveSequence:
		return "ProgressiveSequence";
	case ContainerType::Sequence:
		return "Sequence";
	case ContainerType::Selector:
		return "Selector";
	case ContainerType::Parallel:
		return "Parallel";
	case ContainerType::Race:
		return "Race";
	default:
		return "Unknown";
	}
}

inline ContainerType StringToContainerType(const std::string& str) {
	if (str == "SelectorWithRunning") return ContainerType::SelectorWithRunning;
	if (str == "ProgressiveSequence") return ContainerType::ProgressiveSequence;
	if (str == "Sequence") return ContainerType::Sequence;
	if (str == "Selector") return ContainerType::Selector;
	if (str == "Parallel") return ContainerType::Parallel;
	if (str == "Race") return ContainerType::Race;
	return ContainerType::COUNT;
}

inline std::vector<std::string> GetContainerTypesString() {
	std::vector<std::string> containers;
	for (int i = 0; i < static_cast<int>(ContainerType::COUNT); ++i) {
		std::string containerTypeInChar = ContainerTypeToString(static_cast<ContainerType>(i));
		containers.push_back(containerTypeInChar);
	}
	return containers;
}

// Base class for behavior configuration data
struct BehaviorConfig {
	std::string behaviorType;
	virtual ~BehaviorConfig() = default;
	virtual std::unique_ptr<BehaviorConfig> clone() const = 0; // Pure virtual clone method
	virtual std::vector<ConfigField> GetConfigFields() const { return {}; }
	virtual nlohmann::json ToJson(const std::string& monsterId) const = 0;
};

template<typename Derived>
class BehaviorConfigBase : public BehaviorConfig {
public:
	// Helper methods that derived classes can override
	virtual nlohmann::json SerializeSpecific(const std::string& monsterId) const = 0;
	virtual bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) = 0;

public:
	// Auto-generated methods
	nlohmann::json ToJson(const std::string& monsterId) const final {
		nlohmann::json result;
		result["type"] = behaviorType;

		auto specific = SerializeSpecific(monsterId);
		result.merge_patch(specific);

		return result;
	}

	static std::unique_ptr<BehaviorConfig> FromJson(const nlohmann::json& json,
		const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) {
		auto instance = std::make_unique<Derived>();
		if (instance->DeserializeSpecific(json, monsterId, bulletConfigs)) {
			return instance;
		}
		return nullptr;
	}

};

// BehaviorChaseConfig
struct BehaviorChaseConfig : public BehaviorConfigBase<BehaviorChaseConfig> {
	int chaseSpeed = 10000;

	BehaviorChaseConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::Chase);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorChaseConfig>();
		copy->chaseSpeed = this->chaseSpeed;
		return copy;
	}


	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["speed"] = chaseSpeed; // Convert to float
		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		if (json.contains("speed")) {
			float speed = json["speed"];
			chaseSpeed = static_cast<int>(speed);
		}
		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		return {
			ConfigField("Chase Speed", &const_cast<BehaviorChaseConfig*>(this)->chaseSpeed)
		};
	}
};

// BehaviorDistanceConditionHelperConfig
struct BehaviorDistanceConditionHelperConfig : public BehaviorConfigBase<BehaviorDistanceConditionHelperConfig> {
	int maxDistance = 10000;
	int minDistance = 10000;

	BehaviorDistanceConditionHelperConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::DistanceConditionHelper);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorDistanceConditionHelperConfig>();
		copy->maxDistance = this->maxDistance;
		copy->minDistance = this->minDistance;
		return copy;
	}


	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["minDistance"] = minDistance; // Convert to float
		behaviorJson["maxDistance"] = maxDistance; // Convert to float
		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		if (json.contains("minDistance")) {
			float minDist = json["minDistance"];
			minDistance = static_cast<int>(minDist);
		}
		if (json.contains("maxDistance")) {
			float maxDist = json["maxDistance"];
			maxDistance = static_cast<int>(maxDist);
		}
		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		return {
			ConfigField("Max distance", &const_cast<BehaviorDistanceConditionHelperConfig*>(this)->maxDistance),
			ConfigField("Min distance", &const_cast<BehaviorDistanceConditionHelperConfig*>(this)->minDistance)
		};
	}
};

// BehaviorMovementBounceConfig
struct BehaviorMovementBounceConfig : public BehaviorConfigBase<BehaviorMovementBounceConfig> {
	BehaviorMovementBounceConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::MovementBounce);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		return std::make_unique<BehaviorMovementBounceConfig>();
	}


	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["behaviorType"] = behaviorType;
		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		// No specific fields to deserialize for this behavior
		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		return {}; // No configuration fields
	}
};

// BehaviorShootBarrageConfig
struct BehaviorShootBarrageConfig : public BehaviorConfigBase<BehaviorShootBarrageConfig> {
	int coolDown = 10000;
	int numOfBullet = 10000;
	int spreadAngle = 10000;
	std::string bulletId = "";

	BehaviorShootBarrageConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::ShootBarrage);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorShootBarrageConfig>();
		copy->coolDown = this->coolDown;
		copy->numOfBullet = this->numOfBullet;
		copy->spreadAngle = this->spreadAngle;
		copy->bulletId = this->bulletId;
		return copy;
	}


	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["cooldown"] = coolDown; // Convert to float

		// Use the bullet ID directly
		if (!bulletId.empty()) {
			behaviorJson["bulletID"] = bulletId;
		} else {
			behaviorJson["bulletID"] = "bullet_straight_01"; // Default bullet ID
		}
		behaviorJson["enemyID"] = monsterId;
		behaviorJson["bulletCount"] = numOfBullet;
		behaviorJson["spreadAngle"] = spreadAngle;

		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		if (json.contains("cooldown")) {
			float cooldown = json["cooldown"];
			coolDown = static_cast<int>(cooldown);
		}
		if (json.contains("bulletCount")) {
			numOfBullet = json["bulletCount"];
		}
		if (json.contains("spreadAngle")) {
			spreadAngle = json["spreadAngle"];
		}

		// Store bullet ID directly
		if (json.contains("bulletID")) {
			bulletId = json["bulletID"];
		}

		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		auto* nonConstThis = const_cast<BehaviorShootBarrageConfig*>(this);
		return {
			ConfigField("CoolDown", &nonConstThis->coolDown),
			ConfigField("NumOfBullet", &nonConstThis->numOfBullet),
			ConfigField("SpreadAngle", &nonConstThis->spreadAngle),
			ConfigField("Bullet ID", &nonConstThis->bulletId, ConfigFieldType::BulletID)
		};
	}
};

// BehaviorShootProjectileConfig
struct BehaviorShootProjectileConfig : public BehaviorConfigBase<BehaviorShootProjectileConfig> {
	int coolDown = 10000;
	std::string bulletId = "";

	BehaviorShootProjectileConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::ShootProjectile);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorShootProjectileConfig>();
		copy->coolDown = this->coolDown;
		copy->bulletId = this->bulletId;
		return copy;
	}


	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["cooldown"] = coolDown; // Convert to float

		// Use the bullet ID directly
		if (!bulletId.empty()) {
			behaviorJson["bulletID"] = bulletId;
		} else {
			behaviorJson["bulletID"] = "bullet_straight_01"; // Default bullet ID
		}
		behaviorJson["enemyID"] = monsterId;

		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		if (json.contains("cooldown")) {
			float cooldown = json["cooldown"];
			coolDown = static_cast<int>(cooldown);
		}

		// Store bullet ID directly
		if (json.contains("bulletID")) {
			bulletId = json["bulletID"];
		}

		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		auto* nonConstThis = const_cast<BehaviorShootProjectileConfig*>(this);
		return {
			ConfigField("CoolDown", &nonConstThis->coolDown),
			ConfigField("Bullet ID", &nonConstThis->bulletId, ConfigFieldType::BulletID)
		};
	}
};

// BehaviorShootStrategyBaseConfig
struct BehaviorShootStrategyBaseConfig : public BehaviorConfigBase<BehaviorShootStrategyBaseConfig> {
	BehaviorShootStrategyBaseConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::ShootStrategyBase);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		return std::make_unique<BehaviorShootStrategyBaseConfig>();
	}


	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["behaviorType"] = behaviorType;
		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		// No specific fields to deserialize for this behavior
		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		return {}; // No configuration fields
	}
};

// BehaviorSpreadShotConfig
struct BehaviorSpreadShotConfig : public BehaviorConfigBase<BehaviorSpreadShotConfig> {
	int coolDown = 10000;
	int numOfBullet = 10000;
	int spreadAngle = 10000;
	std::string bulletId = "";

	BehaviorSpreadShotConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::SpreadShot);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorSpreadShotConfig>();
		copy->coolDown = this->coolDown;
		copy->numOfBullet = this->numOfBullet;
		copy->spreadAngle = this->spreadAngle;
		copy->bulletId = this->bulletId;
		return copy;
	}

	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["cooldown"] = coolDown; // Convert to float

		// Use the bullet ID directly
		if (!bulletId.empty()) {
			behaviorJson["bulletID"] = bulletId;
		} else {
			behaviorJson["bulletID"] = "bullet_straight_01"; // Default bullet ID
		}
		behaviorJson["enemyID"] = monsterId;
		behaviorJson["bulletCount"] = numOfBullet;
		behaviorJson["spreadAngle"] = spreadAngle;

		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {
		if (json.contains("cooldown")) {
			float cooldown = json["cooldown"];
			coolDown = static_cast<int>(cooldown);
		}
		if (json.contains("bulletCount")) {
			numOfBullet = json["bulletCount"];
		}
		if (json.contains("spreadAngle")) {
			spreadAngle = json["spreadAngle"];
		}

		// Store bullet ID directly
		if (json.contains("bulletID")) {
			bulletId = json["bulletID"];
		}
		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {
		auto* nonConstThis = const_cast<BehaviorSpreadShotConfig*>(this);
		return {
			ConfigField("CoolDown", &nonConstThis->coolDown),
			ConfigField("NumOfBullet", &nonConstThis->numOfBullet),
			ConfigField("SpreadAngle", &nonConstThis->spreadAngle),
			ConfigField("Bullet ID", &nonConstThis->bulletId, ConfigFieldType::BulletID)
		};
	}
};

struct BehaviorMultiConfig : public BehaviorConfigBase<BehaviorMultiConfig> {
	ContainerType containerType = ContainerType::SelectorWithRunning;
	std::vector<std::unique_ptr<BehaviorConfig>> childBehaviors;

	BehaviorMultiConfig() {
		behaviorType = BehaviorTypeToString(BehaviorType::MultiConfig);
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorMultiConfig>();
		copy->containerType = this->containerType;

		for (const auto& child : childBehaviors) {
			if (child) {
				copy->childBehaviors.push_back(child->clone());
			}
		}

		return copy;
	}

	nlohmann::json SerializeSpecific(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;

		// Map container type to behaviorType string

		behaviorJson["behaviorType"] = ContainerTypeToString(containerType);
		// Process child behaviors recursively without registry
		nlohmann::json behaviorsArray = nlohmann::json::array();
		for (const auto& childBehavior : childBehaviors) {
			if (childBehavior) {
				// Direct serialization using the child's ToJson method
				nlohmann::json childJson = childBehavior->ToJson(monsterId);
				behaviorsArray.push_back(childJson);
			}
		}
		behaviorJson["behaviors"] = behaviorsArray;

		return behaviorJson;
	}

	bool DeserializeSpecific(const nlohmann::json& json, const std::string& monsterId,
		const std::unordered_map<std::string, BulletConfig>& bulletConfigs) override {

		// Set container type based on behaviorType field
		if (json.contains("behaviorType")) {
			std::string behaviorType = json["behaviorType"];
			containerType = StringToContainerType(behaviorType);
		}

		// Clear existing child behaviors
		childBehaviors.clear();

		// Process child behaviors if present
		if (json.contains("behaviors") && json["behaviors"].is_array()) {
			for (const auto& childJson : json["behaviors"]) {
				if (childJson.contains("type")) {
					std::string type = childJson["type"];

					// Direct factory creation without registry
					std::unique_ptr<BehaviorConfig> childBehavior = nullptr;

					if (type == "Chase") {
						auto chase = std::make_unique<BehaviorChaseConfig>();
						if (chase->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(chase);
						}
					}
					else if (type == "DistanceConditionHelper") {
						auto distance = std::make_unique<BehaviorDistanceConditionHelperConfig>();
						if (distance->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(distance);
						}
					}
					else if (type == "MovementBounce") {
						auto bounce = std::make_unique<BehaviorMovementBounceConfig>();
						if (bounce->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(bounce);
						}
					}
					else if (type == "ShootProjectile") {
						auto projectile = std::make_unique<BehaviorShootProjectileConfig>();
						if (projectile->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(projectile);
						}
					}
					else if (type == "SpreadShot") {
						auto spread = std::make_unique<BehaviorSpreadShotConfig>();
						if (spread->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(spread);
						}
					}
					else if (type == "ShootBarrage") {
						auto barrage = std::make_unique<BehaviorShootBarrageConfig>();
						if (barrage->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(barrage);
						}
					}
					else if (type == "ShootStrategyBase") {
						auto strategy = std::make_unique<BehaviorShootStrategyBaseConfig>();
						if (strategy->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(strategy);
						}
					}
					else if (type == "MultiConfig") {
						auto multi = std::make_unique<BehaviorMultiConfig>();
						if (multi->DeserializeSpecific(childJson, monsterId, bulletConfigs)) {
							childBehavior = std::move(multi);
						}
					}

					if (childBehavior) {
						childBehaviors.push_back(std::move(childBehavior));
					}
				}
			}
		}

		return true;
	}

	std::vector<ConfigField> GetConfigFields() const override {

		int* containerTypePtr = reinterpret_cast<int*>(&const_cast<BehaviorMultiConfig*>(this)->containerType);

		return {
			ConfigField("Container Type", containerTypePtr, GetContainerTypesString())

		};
	}
};

// Main monster properties struct
struct MonsterProperties {
	// Basic stats
	std::string name = "";
	MonsterType monsterType = MonsterType::Normal;
	std::string valideMonsterIngame = "";
	int hp = 10000;
	float speed = 10.0f;
	float knockbackResistance = 1.0f;
	int collisionDamage = 10000;

	// Behavior tree configuration
	std::unique_ptr<BehaviorMultiConfig> rootBehavior;

	// Constructor
	MonsterProperties() {
		rootBehavior = std::make_unique<BehaviorMultiConfig>();
	}

	// Copy constructor - NOW WITH PROPER DEEP COPY
	MonsterProperties(const MonsterProperties& other)
		: name(other.name)
		, monsterType(other.monsterType)
		, hp(other.hp)
		, speed(other.speed)
		, knockbackResistance(other.knockbackResistance)
		, collisionDamage(other.collisionDamage)
		, valideMonsterIngame(other.valideMonsterIngame) {

		// Deep copy the behavior tree
		if (other.rootBehavior) {
			rootBehavior = std::unique_ptr<BehaviorMultiConfig>(
				static_cast<BehaviorMultiConfig*>(other.rootBehavior->clone().release())
			);
		}
		else {
			rootBehavior = std::make_unique<BehaviorMultiConfig>();
		}
	}

	// Assignment operator - NOW WITH PROPER DEEP COPY
	MonsterProperties& operator=(const MonsterProperties& other) {
		if (this != &other) {
			name = other.name;
			monsterType = other.monsterType;
			hp = other.hp;
			speed = other.speed;
			knockbackResistance = other.knockbackResistance;
			collisionDamage = other.collisionDamage;

			// Deep copy the behavior tree
			if (other.rootBehavior) {
				rootBehavior = std::unique_ptr<BehaviorMultiConfig>(
					static_cast<BehaviorMultiConfig*>(other.rootBehavior->clone().release())
				);
			}
			else {
				rootBehavior = std::make_unique<BehaviorMultiConfig>();
			}
		}
		return *this;
	}

	// Utility methods
	void ResetToDefaults() {
		name = "";
		monsterType = MonsterType::Normal;
		hp = 10000;
		speed = 10.0f;
		knockbackResistance = 1.0f;
		collisionDamage = 10000;
		rootBehavior = std::make_unique<BehaviorMultiConfig>();
	}

	// Move constructor and assignment
	MonsterProperties(MonsterProperties&&) = default;
	MonsterProperties& operator=(MonsterProperties&&) = default;
};


// Factory function to create behavior configs
inline std::unique_ptr<BehaviorConfig> CreateBehaviorConfig(BehaviorType type) {
	switch (type) {
	case BehaviorType::Chase:
		return std::make_unique<BehaviorChaseConfig>();
	case BehaviorType::DistanceConditionHelper:
		return std::make_unique<BehaviorDistanceConditionHelperConfig>();
	case BehaviorType::MovementBounce:
		return std::make_unique<BehaviorMovementBounceConfig>();
	case BehaviorType::ShootBarrage:
		return std::make_unique<BehaviorShootBarrageConfig>();
	case BehaviorType::ShootProjectile:
		return std::make_unique<BehaviorShootProjectileConfig>();
	case BehaviorType::ShootStrategyBase:
		return std::make_unique<BehaviorShootStrategyBaseConfig>();
	case BehaviorType::SpreadShot:
		return std::make_unique<BehaviorSpreadShotConfig>();
	case BehaviorType::MultiConfig:
		return std::make_unique<BehaviorMultiConfig>();
	default:
		return nullptr;
	}
}

struct PlacedMonster {
	MonsterItem item;
	MonsterProperties properties;
	Vec2 positionInGrid = { 0,0 };
	Vec2 worldPosition = { 0,0 };
	PlacedMonster(const MonsterItem& monsterItem)
		: item(monsterItem) {
	};
	PlacedMonster(const MonsterItem& monsterItem, const MonsterProperties prop)
		: item(monsterItem), properties(prop) {
	};
};

// New structure for monster type definitions
struct MonsterTypeDefinition {
	MonsterItem item;                    // Basic info (id, name)
	MonsterProperties defaultProperties; // Default stats and behavior
	std::string textureName;            // Asset path

	MonsterTypeDefinition() : item({ -1,"" }), textureName("")
	{

	};

	MonsterTypeDefinition(int id, const std::string& validMonster, const std::string& name, const std::string texName)
		: item{ id, name }, textureName(texName) {
		defaultProperties.name = name;
		defaultProperties.hp = 200; // Default HP
		defaultProperties.speed = 10.0; // Default speed
		defaultProperties.knockbackResistance = 1; // Default knockback resistance
		defaultProperties.collisionDamage = 10; // Default collision damage
		defaultProperties.rootBehavior = std::make_unique<BehaviorMultiConfig>();
		defaultProperties.valideMonsterIngame = validMonster;// Initialize with an empty behavior tree
	}

	MonsterTypeDefinition(const MonsterTypeDefinition& def)
		: item(def.item),
		defaultProperties(def.defaultProperties),
		textureName(def.textureName) {
	} // copy constructor
};
