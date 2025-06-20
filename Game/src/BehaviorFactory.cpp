#include "BehaviorFactory.h"
#include "utilities/Logger.h"

BehaviorFactory& BehaviorFactory::GetInstance() {
	static BehaviorFactory instance;
	return instance;
}

void BehaviorFactory::RegisterBehaviorType(const std::string& typeName, BehaviorCreator creator) {
	if (m_behaviorCreators.find(typeName) != m_behaviorCreators.end()) {
		LOG_INFO("Behavior type already registered: " + typeName);
		return;
	}

	m_behaviorCreators[typeName] = creator;
	LOG_INFO("Registered behavior type: " + typeName);
}

std::unique_ptr<BehaviorConfig> BehaviorFactory::CreateBehavior(const std::string& typeName) const {
	auto it = m_behaviorCreators.find(typeName);
	if (it != m_behaviorCreators.end()) {
		return it->second();
	}

	LOG_ERROR("Unknown behavior type: " + typeName);
	return nullptr;
}

std::vector<std::string> BehaviorFactory::GetRegisteredTypes() const {
	std::vector<std::string> types;
	for (const auto& pair : m_behaviorCreators) {
		types.push_back(pair.first);
	}
	return types;
}