#pragma once  
#include <string>  
#include <unordered_map>  
#include <iostream>
#include <vector>
#include <memory>

// Define the static member variable outside the struct  
static std::unordered_map<std::string, const char*> MonsterConfig = {
	{"Slime1",	"assets//Slug.bmp"},
	{"Slime2",	"assets//Snowman.bmp"},
	{"Slime3",	"assets/Spicatus.bmp"},
	{"Boss1",	"assets//Stabbycrab.bmp"},
	{"Boss2",	"assets//Starfish.bmp"},
	{"Boss3",	"assets//Staruss.bmp"},
	{"Boss5",	"assets//Stellectric.bmp"},
};

struct MonsterItem {
	int id;
	std::string name;
};

// Enums for various property types
enum class MonsterType {
	Normal = 0,
	Boss = 1
};

enum class BulletType {
	Straight = 0,
	Parabol = 1,
	Mortal = 2,
	Boss = 3
};

enum class ContainerType {
	SelectorWithRunning = 0,
	ProgressiveSequence = 1,
	Sequence = 2
};

struct BulletConfig {
	BulletType bulletType = BulletType::Parabol;
	int speed = 0;
	int aliveTime = 0;
	int damage = 0;
	int bounce = 0;

	// Copy constructor and assignment operator for BulletConfig
	BulletConfig() = default;
	BulletConfig(const BulletConfig&) = default;
	BulletConfig& operator=(const BulletConfig&) = default;
};

// Base class for behavior configuration data
struct BehaviorConfig {
	std::string behaviorType;
	virtual ~BehaviorConfig() = default;
	virtual std::unique_ptr<BehaviorConfig> clone() const = 0; // Pure virtual clone method
};

// Specific behavior configurations
struct BehaviorChaseConfig : public BehaviorConfig {
	int chaseSpeed = 10000;

	BehaviorChaseConfig() {
		behaviorType = "BehaviorChase";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorChaseConfig>();
		copy->chaseSpeed = this->chaseSpeed;
		return copy;
	}
};

struct BehaviorDistanceConditionHelperConfig : public BehaviorConfig {
	int maxDistance = 10000;
	int minDistance = 10000;

	BehaviorDistanceConditionHelperConfig() {
		behaviorType = "BehaviorDistanceConditionHelper";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorDistanceConditionHelperConfig>();
		copy->maxDistance = this->maxDistance;
		copy->minDistance = this->minDistance;
		return copy;
	}
};

struct BehaviorMovementBounceConfig : public BehaviorConfig {
	BehaviorMovementBounceConfig() {
		behaviorType = "BehaviorMovementBounce";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		return std::make_unique<BehaviorMovementBounceConfig>();
	}
};

struct BehaviorShootBarrageConfig : public BehaviorConfig {
	int coolDown = 10000;
	int numOfBullet = 10000;
	int spreadAngle = 10000;
	BulletConfig bulletConfig;

	BehaviorShootBarrageConfig() {
		behaviorType = "BehaviorShootBarrage";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorShootBarrageConfig>();
		copy->coolDown = this->coolDown;
		copy->numOfBullet = this->numOfBullet;
		copy->spreadAngle = this->spreadAngle;
		copy->bulletConfig = this->bulletConfig;
		return copy;
	}
};

struct BehaviorShootProjectileConfig : public BehaviorConfig {
	int coolDown = 10000;
	BulletConfig bulletConfig;

	BehaviorShootProjectileConfig() {
		behaviorType = "BehaviorShootProjectile";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorShootProjectileConfig>();
		copy->coolDown = this->coolDown;
		copy->bulletConfig = this->bulletConfig;
		return copy;
	}
};

struct BehaviorShootStrategyBaseConfig : public BehaviorConfig {
	BehaviorShootStrategyBaseConfig() {
		behaviorType = "BehaviorShootStrategyBase";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		return std::make_unique<BehaviorShootStrategyBaseConfig>();
	}
};

struct BehaviorSpreadShotConfig : public BehaviorConfig {
	int coolDown = 10000;
	int numOfBullet = 10000;
	int spreadAngle = 10000;
	BulletConfig bulletConfig;

	BehaviorSpreadShotConfig() {
		behaviorType = "BehaviorSpreadShot";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorSpreadShotConfig>();
		copy->coolDown = this->coolDown;
		copy->numOfBullet = this->numOfBullet;
		copy->spreadAngle = this->spreadAngle;
		copy->bulletConfig = this->bulletConfig;
		return copy;
	}
};

struct BehaviorMultiConfig : public BehaviorConfig {
	ContainerType containerType = ContainerType::SelectorWithRunning;
	std::vector<std::unique_ptr<BehaviorConfig>> childBehaviors;

	BehaviorMultiConfig() {
		behaviorType = "BehaviorMultiConfig";
	}

	std::unique_ptr<BehaviorConfig> clone() const override {
		auto copy = std::make_unique<BehaviorMultiConfig>();
		copy->containerType = this->containerType;

		// Deep copy all child behaviors
		for (const auto& child : childBehaviors) {
			if (child) {
				copy->childBehaviors.push_back(child->clone());
			}
		}

		return copy;
	}
};

// Main monster properties struct
struct MonsterProperties {
	// Basic stats
	std::string name = "";
	MonsterType monsterType = MonsterType::Normal;
	int hp = 10000;
	int speed = 10000;
	int knockbackResistance = 10000;
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
		, collisionDamage(other.collisionDamage) {

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
		speed = 10000;
		knockbackResistance = 10000;
		collisionDamage = 10000;
		rootBehavior = std::make_unique<BehaviorMultiConfig>();
	}

	// Move constructor and assignment
	MonsterProperties(MonsterProperties&&) = default;
	MonsterProperties& operator=(MonsterProperties&&) = default;
};

// Helper functions for enum conversions
inline const char* MonsterTypeToString(MonsterType type) {
	switch (type) {
	case MonsterType::Boss: return "Boss";
	case MonsterType::Normal: return "Normal";
	default: return "Unknown";
	}
}

inline const char* BulletTypeToString(BulletType type) {
	switch (type) {
	case BulletType::Straight: return "straight";
	case BulletType::Parabol: return "parabol";
	case BulletType::Mortal: return "mortal";
	case BulletType::Boss: return "boss";
	default: return "unknown";
	}
}

inline const char* ContainerTypeToString(ContainerType type) {
	switch (type) {
	case ContainerType::SelectorWithRunning: return "SelectorWithRunning";
	case ContainerType::ProgressiveSequence: return "ProgressiveSequence";
	case ContainerType::Sequence: return "Sequence";
	default: return "Unknown";
	}
}

// Factory function to create behavior configs
inline std::unique_ptr<BehaviorConfig> CreateBehaviorConfig(const std::string& behaviorType) {
	if (behaviorType == "BehaviorChase") {
		return std::make_unique<BehaviorChaseConfig>();
	}
	else if (behaviorType == "BehaviorDistanceConditionHelper") {
		return std::make_unique<BehaviorDistanceConditionHelperConfig>();
	}
	else if (behaviorType == "BehaviorMovementBounce") {
		return std::make_unique<BehaviorMovementBounceConfig>();
	}
	else if (behaviorType == "BehaviorShootBarrage") {
		return std::make_unique<BehaviorShootBarrageConfig>();
	}
	else if (behaviorType == "BehaviorShootProjectile") {
		return std::make_unique<BehaviorShootProjectileConfig>();
	}
	else if (behaviorType == "BehaviorShootStrategyBase") {
		return std::make_unique<BehaviorShootStrategyBaseConfig>();
	}
	else if (behaviorType == "BehaviorSpreadShot") {
		return std::make_unique<BehaviorSpreadShotConfig>();
	}
	else if (behaviorType == "BehaviorMultiConfig") {
		return std::make_unique<BehaviorMultiConfig>();
	}
	return nullptr;
}

struct PlacedMonster {
	MonsterItem item;
	MonsterProperties properties;
	PlacedMonster(const MonsterItem& monsterItem)
		: item(monsterItem) {
	};
};

// New structure for monster type definitions
struct MonsterTypeDefinition {
	MonsterItem item;                    // Basic info (id, name)
	MonsterProperties defaultProperties; // Default stats and behavior
	const char* texturePath;            // Asset path

	MonsterTypeDefinition(int id, const std::string& name, const char* path)
		: item{ id, name }, texturePath(path) {
		defaultProperties.name = name;
		defaultProperties.hp = 10000; // Default HP
		defaultProperties.speed = 10000; // Default speed
		defaultProperties.knockbackResistance = 10000; // Default knockback resistance
		defaultProperties.collisionDamage = 10000; // Default collision damage
		defaultProperties.rootBehavior = std::make_unique<BehaviorMultiConfig>(); // Initialize with an empty behavior tree
	}
};