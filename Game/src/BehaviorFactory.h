#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include "BehaviorConfig.h"

/**
 * Factory for creating behavior objects
 */
class BehaviorFactory {
public:
	using BehaviorCreator = std::function<std::unique_ptr<BehaviorConfig>()>;

	static BehaviorFactory& GetInstance();

	void RegisterBehaviorType(const std::string& typeName, BehaviorCreator creator);

	template<typename T>
	bool RegisterType(const std::string& typeName) {
		RegisterBehaviorType(typeName, []() -> std::unique_ptr<BehaviorConfig> {
			return std::make_unique<T>();
			});
		return true;
	}

	std::unique_ptr<BehaviorConfig> CreateBehavior(const std::string& typeName) const;
	std::vector<std::string> GetRegisteredTypes() const;

private:
	BehaviorFactory() = default;
	std::unordered_map<std::string, BehaviorCreator> m_behaviorCreators;
};