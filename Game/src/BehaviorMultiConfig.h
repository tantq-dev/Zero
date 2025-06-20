#pragma once
#include "BehaviorConfig.h"
#include <vector>
#include <memory>
#include <BehaviorFactory.h>

//! When have new container type should add it to this 
//! Also add case for switch-case `Serialize(const std::string& monsterId)` and `Deserialize(const nlohmann::json& json)` 
enum class ContainerType {
	SelectorWithRunning = 0,
	ProgressiveSequence,
	Sequence,
	Selector,
	Parallel,
	Race
};

class BehaviorMultiConfig : public BehaviorConfig {
public:
	BehaviorMultiConfig() = default;
	virtual ~BehaviorMultiConfig() = default;

	// BehaviorConfig interface implementation
	std::string GetBehaviorType() const override { return "BehaviorMultiConfig"; }

	nlohmann::json Serialize(const std::string& monsterId) const override {
		nlohmann::json behaviorJson;
		behaviorJson["type"] = GetBehaviorType();

		// Serialize container type
		std::string behaviorType;
		switch (containerType) {
		case ContainerType::SelectorWithRunning: behaviorType = "SelectorWithRunning"; break;
		case ContainerType::ProgressiveSequence: behaviorType = "ProgressiveSequence"; break;
		case ContainerType::Sequence: behaviorType = "Sequence"; break;
		case ContainerType::Selector: behaviorType = "Selector"; break;
		case ContainerType::Parallel: behaviorType = "Parallel"; break;
		case ContainerType::Race: behaviorType = "Race"; break;
		default: behaviorType = "Sequence"; break;
		}
		behaviorJson["behaviorType"] = behaviorType;

		// Serialize child behaviors
		nlohmann::json behaviorsArray = nlohmann::json::array();
		for (const auto& childBehavior : childBehaviors) {
			if (childBehavior) {
				behaviorsArray.push_back(childBehavior->Serialize(monsterId));
			}
		}
		behaviorJson["behaviors"] = behaviorsArray;

		return behaviorJson;
	}

	bool Deserialize(const nlohmann::json& json) override {
		// Clear existing behaviors
		childBehaviors.clear();

		// Parse container type
		if (json.contains("behaviorType")) {
			std::string behaviorTypeStr = json["behaviorType"];
			if (behaviorTypeStr == "SelectorWithRunning") {
				containerType = ContainerType::SelectorWithRunning;
			}
			else if (behaviorTypeStr == "ProgressiveSequence") {
				containerType = ContainerType::ProgressiveSequence;
			}
			else if (behaviorTypeStr == "Sequence") {
				containerType = ContainerType::Sequence;
			}
			else if (behaviorTypeStr == "Selector") {
				containerType = ContainerType::Selector;
			}
			else if (behaviorTypeStr == "Parallel") {
				containerType = ContainerType::Parallel;
			}
			else if (behaviorTypeStr == "Race") {
				containerType = ContainerType::Race;
			}
		}

		// Parse child behaviors
		if (json.contains("behaviors") && json["behaviors"].is_array()) {
			for (const auto& childJson : json["behaviors"]) {
				auto childBehavior = BehaviorConfig::CreateFromJson(childJson);
				if (childBehavior) {
					childBehaviors.push_back(std::move(childBehavior));
				}
			}
		}

		return true;
	}

	std::unique_ptr<BehaviorConfig> Clone() const override {
		auto clone = std::make_unique<BehaviorMultiConfig>();
		clone->containerType = this->containerType;

		// Deep copy all child behaviors
		for (const auto& childBehavior : this->childBehaviors) {
			if (childBehavior) {
				clone->childBehaviors.push_back(childBehavior->Clone());
			}
		}

		return clone;
	}

	// Behavior-specific properties
	ContainerType containerType = ContainerType::Sequence;
	std::vector<std::unique_ptr<BehaviorConfig>> childBehaviors;

	// Static registration
	static bool RegisterType() {
		return BehaviorFactory::GetInstance().RegisterType<BehaviorMultiConfig>("BehaviorMultiConfig");
	}
};