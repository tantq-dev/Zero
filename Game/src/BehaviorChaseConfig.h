#pragma once
#include "BehaviorConfig.h"
#include <BehaviorFactory.h>

class BehaviorChaseConfig : public BehaviorConfig {
public:
	BehaviorChaseConfig() = default;
	virtual ~BehaviorChaseConfig() = default;

	// BehaviorConfig interface implementation
	std::string GetBehaviorType() const override { return "BehaviorChase"; }

	nlohmann::json Serialize(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["type"] = GetBehaviorType();
		behaviorJson["speed"] = chaseSpeed / 10000.0f; // Convert to float
		return behaviorJson;
	}

	bool Deserialize(const nlohmann::json& json) override {
		if (json.contains("speed")) {
			float speed = json["speed"];
			chaseSpeed = static_cast<int>(speed * 10000.0f); // Convert to internal format
		}
		return true;
	}

	std::unique_ptr<BehaviorConfig> Clone() const override {
		return CloneImpl(this);
	}

	// Behavior-specific properties
	int chaseSpeed = 10000; // Default value

	// Static registration
	static bool RegisterType() {
		return BehaviorFactory::GetInstance().RegisterType<BehaviorChaseConfig>("BehaviorChase");
	}
};