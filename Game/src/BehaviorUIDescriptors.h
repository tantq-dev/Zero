#pragma once
#include "BehaviorUIDescriptor.h"
#include "BehaviorMultiConfig.h"
#include "BehaviorChaseConfig.h"
#include "BehaviorShootProjectileConfig.h"
#include "BulletConfig.h"
#include "PropertyDescriptor.h" // Make sure this is explicitly included
namespace Tool::UI {

	// UI Descriptor for BehaviorMultiConfig
	class BehaviorMultiConfigUI : public BehaviorUIDescriptor {
	public:
		std::vector<std::unique_ptr<PropertyDescriptor>> CreatePropertyDescriptors(BehaviorConfig* config) override {
			std::vector<std::unique_ptr<PropertyDescriptor>> properties;

			auto* typedConfig = dynamic_cast<BehaviorMultiConfig*>(config);
			if (!typedConfig) return properties;

			std::vector<std::string> containerTypes = {
				"SelectorWithRunning", "ProgressiveSequence", "Sequence",
				"Selector", "Parallel", "Race"
			};

			properties.push_back(std::make_unique<EnumPropertyDescriptor<ContainerType>>(
				"Container Type", &typedConfig->containerType, containerTypes,
				"Determines how child behaviors are processed"));

			return properties;
		}

		std::string GetDisplayName() const override { return "Multi Behavior"; }
		bool CanHaveChildren() const override { return true; }
		std::string GetCategory() const override { return "Control"; }
	};

	// UI Descriptor for BehaviorChaseConfig
	class BehaviorChaseConfigUI : public BehaviorUIDescriptor {
	public:
		std::vector<std::unique_ptr<PropertyDescriptor>> CreatePropertyDescriptors(BehaviorConfig* config) override {
			std::vector<std::unique_ptr<PropertyDescriptor>> properties;

			auto* typedConfig = dynamic_cast<BehaviorChaseConfig*>(config);
			if (!typedConfig) return properties;

			// Show raw value divided by 10000 in the UI for better readability
			properties.push_back(std::make_unique<IntPropertyDescriptor>(
				"Chase Speed", &typedConfig->chaseSpeed, 0, 100000,
				"Speed at which the monster chases the player"));

			return properties;
		}

		std::string GetDisplayName() const override { return "Chase"; }
		std::string GetCategory() const override { return "Movement"; }
	};

	// UI Descriptor for BehaviorShootProjectileConfig
	class BehaviorShootProjectileConfigUI : public BehaviorUIDescriptor {
	public:
		std::vector<std::unique_ptr<PropertyDescriptor>> CreatePropertyDescriptors(BehaviorConfig* config) override {
			std::vector<std::unique_ptr<PropertyDescriptor>> properties;

			auto* typedConfig = dynamic_cast<BehaviorShootProjectileConfig*>(config);
			if (!typedConfig) return properties;

			properties.push_back(std::make_unique<IntPropertyDescriptor>(
				"CoolDown", &typedConfig->coolDown, 0, 100000,
				"Time between shots (in game ticks)"));

			// Bullet config properties
			properties.push_back(std::make_unique<EnumPropertyDescriptor<BulletType>>(
				"Bullet Type", &typedConfig->bulletConfig.bulletType,
				std::vector<std::string>{ "Straight", "Parabol", "Mortal", "Boss" },
				"Type of bullet to shoot"));

			properties.push_back(std::make_unique<IntPropertyDescriptor>(
				"Speed", &typedConfig->bulletConfig.speed, 0, 100000,
				"Speed of the bullet"));

			properties.push_back(std::make_unique<IntPropertyDescriptor>(
				"Damage", &typedConfig->bulletConfig.damage, 0, 10000,
				"Damage dealt by the bullet"));

			properties.push_back(std::make_unique<IntPropertyDescriptor>(
				"Alive Time", &typedConfig->bulletConfig.aliveTime, 0, 100000,
				"How long the bullet exists before disappearing"));

			properties.push_back(std::make_unique<IntPropertyDescriptor>(
				"Bounce", &typedConfig->bulletConfig.bounce, 0, 10,
				"Number of times the bullet can bounce"));

			return properties;
		}

		std::string GetDisplayName() const override { return "Shoot Projectile"; }
		std::string GetCategory() const override { return "Attack"; }
	};

} // namespace Tool::UI