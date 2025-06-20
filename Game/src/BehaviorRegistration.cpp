#include "BehaviorMultiConfig.h"
#include "BehaviorChaseConfig.h"
#include "BehaviorShootProjectileConfig.h"
#include "BehaviorRegistration.h"
// Include other behavior headers as needed

// Register all behaviors with the factory
bool BehaviorRegistration::RegisterAllBehaviorTypes() {
	bool success = true;

	// Register behavior types
	success &= BehaviorMultiConfig::RegisterType();
	success &= BehaviorChaseConfig::RegisterType();
	success &= BehaviorShootProjectileConfig::RegisterType();

	// Add more registrations here as you implement more behavior types

	return success;
}