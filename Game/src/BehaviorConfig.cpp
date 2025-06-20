#include "BehaviorConfig.h"
#include "BehaviorFactory.h"
#include "utilities/Logger.h"

std::unique_ptr<BehaviorConfig> BehaviorConfig::CreateFromJson(const nlohmann::json& json) {
	if (!json.contains("type")) {
		LOG_ERROR("JSON missing 'type' field for behavior");
		return nullptr;
	}

	std::string behaviorType = json["type"];
	auto behavior = BehaviorFactory::GetInstance().CreateBehavior(behaviorType);

	if (!behavior) {
		LOG_ERROR("Failed to create behavior of type: " + behaviorType);
		return nullptr;
	}

	if (!behavior->Deserialize(json)) {
		LOG_ERROR("Failed to deserialize behavior of type: " + behaviorType);
		return nullptr;
	}

	return behavior;
}

std::unique_ptr<BehaviorConfig> BehaviorConfig::CreateFromType(const std::string& behaviorType) {
	return BehaviorFactory::GetInstance().CreateBehavior(behaviorType);
}