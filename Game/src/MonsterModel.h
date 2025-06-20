#pragma once  
#include <string>  
#include <unordered_map>  
#include <iostream>
#include <vector>
#include <memory>
#include "utilities/Vec2.h"
#include <utilities/Logger.h>
#include "BulletConfig.h"
#include <BehaviorMultiConfig.h>
#include <BehaviorChaseConfig.h>


static std::unordered_map<std::string, const char*> BulletTextureMap = {
	{"bullet_01", ""},
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
	Normal = 0,
	Boss = 1
};


// Main monster properties struct
struct MonsterProperties {
	// Basic stats
	std::string name = "";
	MonsterType monsterType = MonsterType::Normal;
	std::string valideMonsterIngame = "";
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
		, collisionDamage(other.collisionDamage)
		, valideMonsterIngame(other.valideMonsterIngame) {

		// Deep copy the behavior tree
		if (other.rootBehavior) {
			rootBehavior = std::unique_ptr<BehaviorMultiConfig>(
				static_cast<BehaviorMultiConfig*>(other.rootBehavior->Clone().release())
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
					static_cast<BehaviorMultiConfig*>(other.rootBehavior->Clone().release())
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
		defaultProperties.hp = 10000; // Default HP
		defaultProperties.speed = 10000; // Default speed
		defaultProperties.knockbackResistance = 10000; // Default knockback resistance
		defaultProperties.collisionDamage = 10000; // Default collision damage
		defaultProperties.rootBehavior = std::make_unique<BehaviorMultiConfig>();
		defaultProperties.valideMonsterIngame = validMonster;// Initialize with an empty behavior tree
	}

	MonsterTypeDefinition(const MonsterTypeDefinition& def)
		: item(def.item),
		defaultProperties(def.defaultProperties),
		textureName(def.textureName) {
	} // copy constructor
};