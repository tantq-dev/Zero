#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include "PropertyDescriptor.h"
#include "BehaviorConfig.h"

namespace Tool::UI {

	/**
	 * UI descriptor for a behavior type
	 */
	class BehaviorUIDescriptor {
	public:
		virtual ~BehaviorUIDescriptor() = default;

		// Create property descriptors for this behavior type
		virtual std::vector<std::unique_ptr<PropertyDescriptor>> CreatePropertyDescriptors(BehaviorConfig* config) = 0;

		// Get display name for UI
		virtual std::string GetDisplayName() const = 0;

		// Whether this behavior can have child behaviors
		virtual bool CanHaveChildren() const { return false; }

		// Optional: Get category for organizing in UI
		virtual std::string GetCategory() const { return "General"; }
	};

	/**
	 * Factory for behavior UI descriptors
	 */
	class BehaviorUIFactory {
	public:
		using UIDescriptorCreator = std::function<std::unique_ptr<BehaviorUIDescriptor>()>;

		static BehaviorUIFactory& GetInstance();

		void RegisterBehaviorUI(const std::string& behaviorType, UIDescriptorCreator creator);
		std::unique_ptr<BehaviorUIDescriptor> CreateBehaviorUIDescriptor(const std::string& behaviorType);
		std::vector<std::string> GetAllBehaviorTypes() const;

	private:
		BehaviorUIFactory() = default;
		std::unordered_map<std::string, UIDescriptorCreator> m_uiDescriptorCreators;
	};

	// Helper template for registering UI descriptors
	template<typename T>
	void RegisterBehaviorUI(const std::string& behaviorType) {
		BehaviorUIFactory::GetInstance().RegisterBehaviorUI(behaviorType,
			[]() -> std::unique_ptr<BehaviorUIDescriptor> { return std::make_unique<T>(); });
	}

} // namespace Tool::UI