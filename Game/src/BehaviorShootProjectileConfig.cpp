#include "BehaviorShootProjectileConfig.h"
#include "BehaviorFactory.h"
#include <string>

// Register with factory
bool BehaviorShootProjectileConfig::RegisterType() {
	return BehaviorFactory::GetInstance().RegisterType<BehaviorShootProjectileConfig>("BehaviorShootProjectile");
}

// Simple implementation of GetBulletID - in a real implementation, this might be more complex
std::string BehaviorShootProjectileConfig::GetBulletID(const BulletConfig& config) const {
	// Simple implementation - in real code this would match DataHandler's implementation
	std::string bulletType;
	switch (config.bulletType) {
	case BulletType::Straight: bulletType = "straight"; break;
	case BulletType::Parabol: bulletType = "parabol"; break;
	case BulletType::Mortal: bulletType = "mortal"; break;
	case BulletType::Boss: bulletType = "boss"; break;
	default: bulletType = "straight"; break;
	}

	return "bullet_" + bulletType + "_01";
}