#pragma once
#include "MonsterModel.h"
#include "MapEditor.h"
#include <imgui.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace Tool {
	namespace UI {

		struct BehaviorNode {
			int id;  // Unique identifier for the node
			std::string name;
			std::vector<std::unique_ptr<BehaviorNode>> children;  // Changed to unique_ptr for proper ownership
			BehaviorNode* parent = nullptr;  // Raw pointer to parent
			std::unique_ptr<BehaviorConfig> config;  // Store the actual configuration data

			// Constructor
			BehaviorNode(int nodeId, const std::string& nodeName)
				: id(nodeId), name(nodeName) {
			}

			// Helper methods
			bool isRoot() const { return parent == nullptr; }
			bool isLeaf() const { return children.empty(); }

			// Get the root node
			BehaviorNode* getRoot() {
				BehaviorNode* current = this;
				while (current->parent != nullptr) {
					current = current->parent;
				}
				return current;
			}

			// Get path from root to this node
			std::vector<std::string> getPathFromRoot() const {
				std::vector<std::string> path;
				const BehaviorNode* current = this;
				while (current != nullptr) {
					path.insert(path.begin(), current->name);
					current = current->parent;
				}
				return path;
			}

			// Add child node
			BehaviorNode* addChild(int childId, const std::string& childName) {
				auto child = std::make_unique<BehaviorNode>(childId, childName);
				child->parent = this;
				BehaviorNode* childPtr = child.get();
				children.push_back(std::move(child));
				return childPtr;
			}

			// Remove child by index
			void removeChild(size_t index) {
				if (index < children.size()) {
					children.erase(children.begin() + index);
				}
			}

			// Find child index
			int findChildIndex(BehaviorNode* child) const {
				for (size_t i = 0; i < children.size(); ++i) {
					if (children[i].get() == child) {
						return static_cast<int>(i);
					}
				}
				return -1;
			}
		};

		class UIMonsterProperties {
		private:
			std::unique_ptr<BehaviorNode> m_pRootNode;
			std::vector<BehaviorType> availableBehavior = GetAvailableBehaviorTypes();
			int currentSelection = 0;
			int m_nextNodeId = 0;  // For generating unique node IDs
			std::vector<std::string> m_currentBulletIDs;  // Store current bullet IDs
			// Add reference to MapEditor and current properties
			MonsterTypeDefinition* m_pCurrentProperties = nullptr;
			bool m_hasValidSelection = false;
			int m_activeDropdownNodeId = -1;  // Replace m_showBehaviorDropdown with this
			void ShowBehaviorDropdown(BehaviorNode& node);

			// Generic rendering method
			bool RenderConfigFields(const std::vector<ConfigField>& fields);
			bool RenderBulletConfigFields(BulletConfig* bulletConfig);
			std::vector<const char*> GetAvailableBullets() const;
			std::vector<std::string> GetAvailableBulletIds() const;

		public:
			UIMonsterProperties();
			~UIMonsterProperties();

			void ShowUIMonsterProperties(bool* p_open, std::vector<std::string> currentBulletIDs);
			void SetCurrentProperties(MonsterTypeDefinition properties);
			void BehaviorMultiConfigSettingsOnly(BehaviorNode& node);

		private:
			// Node creation and management
			std::unique_ptr<BehaviorNode> CreateBehaviorNode(const BehaviorType type);
			bool m_showBehaviorDropdown = false;
			void InitializeRootNode();
			int GetNextNodeId() { return ++m_nextNodeId; }

			// Rendering methods
			void BehaviorMultipleConfig();
			void RenderBehaviorPanels(BehaviorNode& parentNode);
			void AddBehaviorToNode(BehaviorNode& parent, const BehaviorType type);
			void ShowBehaviorConfiguration(BehaviorNode& node);


			// Data conversion methods
			void ConvertFromMonsterProperties(const MonsterTypeDefinition& properties);
			void ConvertToMonsterProperties(MonsterTypeDefinition& properties);
			void SaveCurrentProperties();
			std::unique_ptr<BehaviorNode> ConvertBehaviorConfigToNode(const BehaviorConfig* config);
			std::unique_ptr<BehaviorConfig> ConvertNodeToBehaviorConfig(const BehaviorNode& node);

		};
	}
}