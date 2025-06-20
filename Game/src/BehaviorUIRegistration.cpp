#include "BehaviorUIDescriptors.h"
#include "BehaviorUIRegistration.h"
namespace Tool::UI {
	// Register all behavior UI descriptors
	bool BehaviorUIRegistration::RegisterBehaviorUIDescriptors() {
		// Register UI descriptors for behaviors
		RegisterBehaviorUI<BehaviorMultiConfigUI>("BehaviorMultiConfig");
		RegisterBehaviorUI<BehaviorChaseConfigUI>("BehaviorChase");
		RegisterBehaviorUI<BehaviorShootProjectileConfigUI>("BehaviorShootProjectile");

		// Add more registrations here as you implement more behavior types

		return true;
	}

} // namespace Tool::UI