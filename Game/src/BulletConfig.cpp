#include "MonsterModel.h"
std::string BulletConfig::GetBulletID(const BulletConfig& bulletConfig)
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