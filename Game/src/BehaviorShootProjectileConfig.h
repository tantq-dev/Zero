#pragma once
#include "BehaviorConfig.h"
#include "BulletConfig.h"

class BehaviorShootProjectileConfig : public BehaviorConfig {
public:
	BehaviorShootProjectileConfig() = default;
	virtual ~BehaviorShootProjectileConfig() = default;

	// BehaviorConfig interface implementation
	std::string GetBehaviorType() const override { return "BehaviorShootProjectile"; }

	nlohmann::json Serialize(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["type"] = GetBehaviorType();
		behaviorJson["cooldown"] = coolDown / 10000.0f; // Convert to float

		// We'll need a way to get/create bullet ID
		std::string bulletId = GetBulletID(bulletConfig);
		behaviorJson["bulletID"] = bulletId;
		behaviorJson["enemyID"] = monsterId;

		return behaviorJson;
	}

	bool Deserialize(const nlohmann::json& json) override {
		if (json.contains("cooldown")) {
			float cooldown = json["cooldown"];
			coolDown = static_cast<int>(cooldown * 10000.0f);
		}

		// Note: Bullet config is typically set externally after deserialization
		// based on bulletID references, which is handled by DataHandler

		return true;
	}

	std::unique_ptr<BehaviorConfig> Clone() const override {
		return CloneImpl(this);
	}

	// Behavior-specific properties
	int coolDown = 20000; // Default value
	BulletConfig bulletConfig;

	// Static registration
	static bool RegisterType();

private:
	// Helper method to get bullet ID (this would typically be implemented in DataHandler)
	std::string GetBulletID(const BulletConfig& config) const;
};