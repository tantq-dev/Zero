#pragma once
#include <string>


enum class BulletType {
	Straight = 0,
	Parabol,
	Mortal,
	Boss
};

class BulletConfig {
public:
	BulletType bulletType = BulletType::Parabol;
	std::string validBulletIngame = "";
	int speed = 0;
	int aliveTime = 0;
	int damage = 0;
	int bounce = 0;

	// Copy constructor and assignment operator for BulletConfig
	BulletConfig() = default;
	BulletConfig(const BulletConfig&) = default;
	BulletConfig& operator=(const BulletConfig&) = default;
};