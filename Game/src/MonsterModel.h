#pragma once  
#include <string>  
#include <unordered_map>  
#include <iostream>

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

// Base class for behavior configuration data
struct BehaviorConfig {
	std::string behaviorType;
	virtual ~BehaviorConfig() = default;
};

// Specific behavior configurations
struct BehaviorChaseConfig : public BehaviorConfig {
	int chaseSpeed = 10000;

	BehaviorChaseConfig() {
		behaviorType = "BehaviorChase";
	}
};

struct BehaviorDistanceConditionHelperConfig : public BehaviorConfig {
	int maxDistance = 10000;
	int minDistance = 10000;

	BehaviorDistanceConditionHelperConfig() {
		behaviorType = "BehaviorDistanceConditionHelper";
	}
};

struct BehaviorMovementBounceConfig : public BehaviorConfig {
	BehaviorMovementBounceConfig() {
		behaviorType = "BehaviorMovementBounce";
	}
};

struct BehaviorShootBarrageConfig : public BehaviorConfig {
	int coolDown = 10000;
	BulletType bulletType = BulletType::Parabol;
	int numOfBullet = 10000;
	int spreadAngle = 10000;

	BehaviorShootBarrageConfig() {
		behaviorType = "BehaviorShootBarrage";
	}
};

struct BehaviorShootProjectileConfig : public BehaviorConfig {
	int coolDown = 10000;
	BulletType bulletType = BulletType::Parabol;

	BehaviorShootProjectileConfig() {
		behaviorType = "BehaviorShootProjectile";
	}
};

struct BehaviorShootStrategyBaseConfig : public BehaviorConfig {
	BehaviorShootStrategyBaseConfig() {
		behaviorType = "BehaviorShootStrategyBase";
	}
};

struct BehaviorSpreadShotConfig : public BehaviorConfig {
	int coolDown = 10000;
	BulletType bulletType = BulletType::Parabol;
	int numOfBullet = 10000;
	int spreadAngle = 10000;

	BehaviorSpreadShotConfig() {
		behaviorType = "BehaviorSpreadShot";
	}
};

struct BehaviorMultiConfig : public BehaviorConfig {
	ContainerType containerType = ContainerType::SelectorWithRunning;
	std::vector<std::unique_ptr<BehaviorConfig>> childBehaviors;

	BehaviorMultiConfig() {
		behaviorType = "BehaviorMultiConfig";
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

	// Copy constructor
	MonsterProperties(const MonsterProperties& other)
		: name(other.name)
		, monsterType(other.monsterType)
		, hp(other.hp)
		, speed(other.speed)
		, knockbackResistance(other.knockbackResistance)
		, collisionDamage(other.collisionDamage) {
		// Deep copy the behavior tree (you'll need to implement this based on your needs)
		rootBehavior = std::make_unique<BehaviorMultiConfig>();
		// TODO: Implement deep copy of behavior tree
	}

	// Assignment operator
	MonsterProperties& operator=(const MonsterProperties& other) {
		if (this != &other) {
			name = other.name;
			monsterType = other.monsterType;
			hp = other.hp;
			speed = other.speed;
			knockbackResistance = other.knockbackResistance;
			collisionDamage = other.collisionDamage;

			rootBehavior = std::make_unique<BehaviorMultiConfig>();
			// TODO: Implement deep copy of behavior tree
		}
		return *this;
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