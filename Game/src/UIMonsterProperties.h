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
			MonsterProperties* m_pCurrentProperties;
			bool m_hasValidSelection = true;

		public:
			UIMonsterProperties();
			~UIMonsterProperties();

			void ShowUIMonsterProperties(bool* p_open);
			void SetCurrentProperties(MonsterProperties& m_currentProperties);
		private:
			void RenderBehaviorTree(BehaviorNode& node);
			void RenderSelectableTree(BehaviorNode& node, BehaviorNode*& selectedNode);
			void BehaviorMultipleConfig();
			void RenderBehaviorPanels(BehaviorNode& parentNode);
			void AddBehaviorToNode(BehaviorNode& parent, const std::string& behaviorName);
			void ShowBehaviorConfiguration(BehaviorNode& node);

			void BehaviorChaseConfigUI(BehaviorChaseConfig* config = nullptr);
			void BehaviorDistanceConditionHelperConfigUI(BehaviorDistanceConditionHelperConfig* config = nullptr);
			void BehaviorMovementBounceConfigUI(BehaviorMovementBounceConfig* config = nullptr);
			void BehaviorShootBarrageConfigUI(BehaviorShootBarrageConfig* config = nullptr);
			void BehaviorShootProjectileConfigUI(BehaviorShootProjectileConfig* config = nullptr);
			void BehaviorShootStrategyBaseConfigUI(BehaviorShootStrategyBaseConfig* config = nullptr);
			void BehaviorSpreadShotConfigUI(BehaviorSpreadShotConfig* config = nullptr);
		};
	}
}