#pragma once
#include "MonsterModel.h"
#include "MapEditor.h"
#include <imgui.h>
#include <vector>
#include <string>

namespace Tool {
	namespace UI {

		struct BehaviorNode {
			std::string name;
			std::vector<BehaviorNode> children;
		};

		class UIMonsterProperties {
		private:
			BehaviorNode rootNode;
			const char* availableItems[8] = {
				"BehaviorChase",
				"BehaviorDistanceConditionHelper",
				"BehaviorMovementBounce",
				"BehaviorShootBarrage",
				"BehaviorShootProjectile",
				"BehaviorShootStrategyBase",
				"BehaviorSpreadShot",
				"BehaviorMultiConfig"
			};
			int currentSelection = 0;

			// Add reference to MapEditor and current properties
			MapEditor* m_mapEditor = nullptr;
			MonsterProperties m_currentProperties;
			bool m_hasValidSelection = false;

		public:
			UIMonsterProperties();
			~UIMonsterProperties();

			void SetMapEditor(MapEditor* mapEditor) { m_mapEditor = mapEditor; }
			void ShowUIMonsterProperties(bool* p_open);

			// Convert between BehaviorNode and MonsterProperties
			void ConvertFromMonsterProperties(const MonsterProperties& properties);
			void ConvertToMonsterProperties(MonsterProperties& properties);

		private:
			// Existing methods...
			void RenderBehaviorTree(BehaviorNode& node);
			void RenderSelectableTree(BehaviorNode& node, BehaviorNode*& selectedNode);
			void BehaviorMultipleConfig();
			void RenderBehaviorPanels(BehaviorNode& parentNode);
			void AddBehaviorToNode(BehaviorNode& parent, const std::string& behaviorName);
			void ShowBehaviorConfiguration(BehaviorNode& node);

			// Behavior config methods (update these to work with properties)
			void BehaviorChaseConfigUI(BehaviorChaseConfig* config = nullptr);
			void BehaviorDistanceConditionHelperConfigUI(BehaviorDistanceConditionHelperConfig* config = nullptr);
			void BehaviorMovementBounceConfigUI(BehaviorMovementBounceConfig* config = nullptr);
			void BehaviorShootBarrageConfigUI(BehaviorShootBarrageConfig* config = nullptr);
			void BehaviorShootProjectileConfigUI(BehaviorShootProjectileConfig* config = nullptr);
			void BehaviorShootStrategyBaseConfigUI(BehaviorShootStrategyBaseConfig* config = nullptr);
			void BehaviorSpreadShotConfigUI(BehaviorSpreadShotConfig* config = nullptr);

			// Helper methods
			void LoadCurrentMonsterProperties();
			void SaveCurrentMonsterProperties();
		};
	}
}