#include "BehaviorUIDescriptor.h"
#include "utilities/Logger.h"

namespace Tool::UI {

	BehaviorUIFactory& BehaviorUIFactory::GetInstance() {
		static BehaviorUIFactory instance;
		return instance;
	}

	void BehaviorUIFactory::RegisterBehaviorUI(const std::string& behaviorType, UIDescriptorCreator creator) {
		if (m_uiDescriptorCreators.find(behaviorType) != m_uiDescriptorCreators.end()) {
			LOG_ERROR("UI descriptor already registered for behavior type: " + behaviorType);
			return;
		}

		m_uiDescriptorCreators[behaviorType] = creator;
		LOG_INFO("Registered UI descriptor for behavior type: " + behaviorType);
	}

	std::unique_ptr<BehaviorUIDescriptor> BehaviorUIFactory::CreateBehaviorUIDescriptor(const std::string& behaviorType) {
		auto it = m_uiDescriptorCreators.find(behaviorType);
		if (it != m_uiDescriptorCreators.end()) {
			return it->second();
		}

		LOG_ERROR("No UI descriptor registered for behavior type: " + behaviorType);
		return nullptr;
	}

	std::vector<std::string> BehaviorUIFactory::GetAllBehaviorTypes() const {
		std::vector<std::string> types;
		for (const auto& pair : m_uiDescriptorCreators) {
			types.push_back(pair.first);
		}
		return types;
	}

} // namespace Tool::UI