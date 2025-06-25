#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <json.hpp>
#include "utilities/Logger.h"
#include "MonsterModel.h"

// Forward declarations
struct BehaviorConfig;
struct BehaviorMultiConfig;

#define REGISTER_BEHAVIOR(BehaviorClass, JsonType) \
    namespace { \
        struct BehaviorClass##Registrar { \
            BehaviorClass##Registrar() { \
                Tool::BehaviorRegistry::Instance().RegisterBehavior( \
                   JsonType, \
                    [](const BehaviorConfig* behavior, const std::string& monsterId) -> nlohmann::json { \
                        return static_cast<const BehaviorClass*>(behavior)->ToJson(monsterId); \
                    }, \
                    [](const nlohmann::json& json, const std::string& monsterId, \
                       const std::unordered_map<std::string, BulletConfig>& bulletConfigs) -> std::unique_ptr<BehaviorConfig> { \
                        return BehaviorClass::FromJson(json, monsterId, bulletConfigs); \
                    } \
                ); \
            } \
        }; \
        static BehaviorClass##Registrar g_##BehaviorClass##Registrar; \
    }

namespace Tool {

	// Behavior registry that maps behavior types to processor and importer functions
	class BehaviorRegistry {
	public:
		using ProcessorFunc = std::function<nlohmann::json(const BehaviorConfig*, const std::string&)>;
		using ImporterFunc = std::function<std::unique_ptr<BehaviorConfig>(const nlohmann::json&, const std::string&, const std::unordered_map<std::string, BulletConfig>&)>;

		static BehaviorRegistry& Instance() {
			static BehaviorRegistry instance;

			return instance;
		}

		// Register a behavior type with its processor and importer functions
		void RegisterBehavior(const std::string& type, ProcessorFunc processor, ImporterFunc importer) {
			m_processors[type] = processor;
			m_importers[type] = importer;
			LOG_INFO("Registered behavior type: " + type);
		}

		// Get the processor function for a behavior type
		ProcessorFunc GetProcessor(const std::string& type) const {
			auto it = m_processors.find(type);
			if (it != m_processors.end()) {
				return it->second;
			}
			return nullptr;
		}

		// Get the importer function for a behavior type
		ImporterFunc GetImporter(const std::string& type) const {
			auto it = m_importers.find(type);
			if (it != m_importers.end()) {
				return it->second;
			}
			return nullptr;
		}

	private:
		BehaviorRegistry() = default; // Private constructor for singleton

		std::unordered_map<std::string, ProcessorFunc> m_processors;
		std::unordered_map<std::string, ImporterFunc> m_importers;

	};

} // namespace Tool

REGISTER_BEHAVIOR(BehaviorChaseConfig, BehaviorTypeToString(BehaviorType::Chase))
REGISTER_BEHAVIOR(BehaviorDistanceConditionHelperConfig, BehaviorTypeToString(BehaviorType::DistanceConditionHelper))
REGISTER_BEHAVIOR(BehaviorMovementBounceConfig, BehaviorTypeToString(BehaviorType::MovementBounce))
REGISTER_BEHAVIOR(BehaviorShootBarrageConfig, BehaviorTypeToString(BehaviorType::ShootBarrage))
REGISTER_BEHAVIOR(BehaviorShootProjectileConfig, BehaviorTypeToString(BehaviorType::ShootProjectile))
REGISTER_BEHAVIOR(BehaviorShootStrategyBaseConfig, BehaviorTypeToString(BehaviorType::ShootStrategyBase))
REGISTER_BEHAVIOR(BehaviorSpreadShotConfig, BehaviorTypeToString(BehaviorType::SpreadShot))
REGISTER_BEHAVIOR(BehaviorMultiConfig, BehaviorTypeToString(BehaviorType::MultiConfig))