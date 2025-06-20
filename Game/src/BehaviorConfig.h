#pragma once
#include <string>
#include <memory>
#include <vector>
#include <json.hpp>

/**
 * Base class for all behavior configurations
 */
class BehaviorConfig {
public:
	virtual ~BehaviorConfig() = default;

	// Core interface methods for polymorphic operations
	virtual std::string GetBehaviorType() const = 0;
	virtual nlohmann::json Serialize(const std::string& monsterId) const = 0;
	virtual bool Deserialize(const nlohmann::json& json) = 0;
	virtual std::unique_ptr<BehaviorConfig> Clone() const = 0;

	// Factory methods for creating behaviors
	static std::unique_ptr<BehaviorConfig> CreateFromJson(const nlohmann::json& json);
	static std::unique_ptr<BehaviorConfig> CreateFromType(const std::string& behaviorType);

	// Helper for derived classes to implement Clone
	template<typename T>
	static std::unique_ptr<BehaviorConfig> CloneImpl(const T* self) {
		auto clone = std::make_unique<T>(*self); // Ensure T has a copy constructor
		return clone;
	}
};