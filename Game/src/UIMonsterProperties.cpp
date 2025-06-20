#include "UIMonsterProperties.h"
#include <imgui.h>
#include <vector>
#include <string>
#include "EventKey.h"
#include "core/EventSystem.h"
#include "utilities/Logger.h"
#include "BulletConfig.h"
#include "BehaviorFactory.h"
#include <BehaviorShootProjectileConfig.h>

Tool::UI::UIMonsterProperties::UIMonsterProperties()
{
}

Tool::UI::UIMonsterProperties::~UIMonsterProperties()
{
}

void Tool::UI::UIMonsterProperties::InitializeRootNode()
{
	m_pRootNode = std::make_unique<BehaviorNode>(GetNextNodeId(), "BehaviorMultiConfig");
	m_pRootNode->config = std::make_unique<BehaviorMultiConfig>();
}

std::unique_ptr<Tool::UI::BehaviorNode> Tool::UI::UIMonsterProperties::CreateBehaviorNode(const std::string& behaviorName)
{
	auto node = std::make_unique<BehaviorNode>(GetNextNodeId(), behaviorName);

	// Only create supported behavior types
	if (behaviorName == "BehaviorChase") {
		node->config = std::make_unique<BehaviorChaseConfig>();
	}
	else if (behaviorName == "BehaviorShootProjectile") {
		node->config = std::make_unique<BehaviorShootProjectileConfig>();
	}
	else if (behaviorName == "BehaviorMultiConfig") {
		node->config = std::make_unique<BehaviorMultiConfig>();
	}
	else {
		LOG_ERROR("Attempted to create unsupported behavior type: " + behaviorName);
		// Create a default MultiConfig as fallback
		node->name = "BehaviorMultiConfig";
		node->config = std::make_unique<BehaviorMultiConfig>();
	}

	return node;
}

void Tool::UI::UIMonsterProperties::ShowUIMonsterProperties(bool* p_open)
{
	if (p_open) {
		bool window_contents_visible = ImGui::Begin("Monster Properties", p_open, ImGuiWindowFlags_MenuBar);
		if (!window_contents_visible) {
			ImGui::End();
			return;
		}

		bool dataChanged = false;  // Track if any data changed

		if (m_hasValidSelection && m_pCurrentProperties) {
			if (ImGui::TreeNode("Stat")) {
				// Name
				char nameBuf[128];
				strncpy(nameBuf, m_pCurrentProperties->defaultProperties.name.c_str(), sizeof(nameBuf));
				nameBuf[sizeof(nameBuf) - 1] = '\0';
				if (ImGui::InputText("Name: ", nameBuf, IM_ARRAYSIZE(nameBuf))) {
					m_pCurrentProperties->defaultProperties.name = nameBuf;
					dataChanged = true;
				}

				// Monster type
				const char* monsterType[] = { "Normal","Boss" };
				int itemType = static_cast<int>(m_pCurrentProperties->defaultProperties.monsterType);
				if (ImGui::Combo("Monster Type", &itemType, monsterType, IM_ARRAYSIZE(monsterType))) {
					m_pCurrentProperties->defaultProperties.monsterType = static_cast<MonsterType>(itemType);
					dataChanged = true;
				}

				// HP
				if (ImGui::InputInt("HP", &m_pCurrentProperties->defaultProperties.hp)) {
					dataChanged = true;
				}

				// Move speed
				if (ImGui::InputInt("Speed", &m_pCurrentProperties->defaultProperties.speed)) {
					dataChanged = true;
				}

				// Knockback resistance
				if (ImGui::InputInt("Knockback resistance", &m_pCurrentProperties->defaultProperties.knockbackResistance)) {
					dataChanged = true;
				}

				// Collision damage
				if (ImGui::InputInt("Collision damage", &m_pCurrentProperties->defaultProperties.collisionDamage)) {
					dataChanged = true;
				}

				ImGui::LabelText("Valid Monster:", m_pCurrentProperties->defaultProperties.valideMonsterIngame.c_str());

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Behavior")) {
				// Store previous child count to detect changes
				size_t previousChildCount = m_pRootNode ? m_pRootNode->children.size() : 0;

				BehaviorMultipleConfig();

				// Check if behavior tree changed
				size_t currentChildCount = m_pRootNode ? m_pRootNode->children.size() : 0;
				if (currentChildCount != previousChildCount) {
					dataChanged = true;
				}

				ImGui::TreePop();
			}

			// Auto-save if any data changed
			if (dataChanged) {
				SaveCurrentProperties();
			}
		}
		else {
			ImGui::Text("No monster selected. Select a monster to edit its properties.");
		}

		ImGui::End();
	}
}

void Tool::UI::UIMonsterProperties::SetCurrentProperties(MonsterTypeDefinition properties)
{
	m_pCurrentProperties = new MonsterTypeDefinition(properties);
	m_hasValidSelection = true;
	m_nextNodeId = 0; // Reset node ID counter

	// Convert monster properties to UI nodes
	ConvertFromMonsterProperties(properties);
}

void Tool::UI::UIMonsterProperties::BehaviorMultipleConfig()
{
	if (!m_pRootNode) {
		InitializeRootNode();
	}

	// Container type configuration for root
	if (auto* multiConfig = dynamic_cast<BehaviorMultiConfig*>(m_pRootNode->config.get())) {
		ImGui::Text("Container Type:");
		static const char* containerTypes[] = {
			"SelectorWithRunning",
			"ProgressiveSequence",
			"Sequence",
			"Selector",
			"Parallel",
			"Race",
		};
		int containerType = static_cast<int>(multiConfig->containerType);
		if (ImGui::Combo("Type", &containerType, containerTypes, IM_ARRAYSIZE(containerTypes))) {
			multiConfig->containerType = static_cast<ContainerType>(containerType);
			SaveCurrentProperties();
		}
		ImGui::Separator();
	}

	// Add new behavior to root
	if (ImGui::Button("Add Behavior")) {
		m_activeDropdownNodeId = m_pRootNode->id;
	}

	// Show behavior selection dropdown for this specific node
	ShowBehaviorDropdown(*m_pRootNode);

	ImGui::Separator();

	// Display behaviors as expandable panels
	ImGui::Text("Behaviors:");
	RenderBehaviorPanels(*m_pRootNode);
}

void Tool::UI::UIMonsterProperties::RenderBehaviorPanels(BehaviorNode& parentNode)
{
	for (int i = 0; i < parentNode.children.size(); i++) {
		BehaviorNode& node = *parentNode.children[i];

		ImGui::PushID(node.id);

		// Create collapsing header for each behavior
		std::string headerText = node.name + " (ID: " + std::to_string(node.id) + ")";
		bool isOpen = ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);

		// Delete button on the same line
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
		if (ImGui::Button("X")) {
			parentNode.removeChild(i);
			ImGui::PopID();
			SaveCurrentProperties(); // Save when a behavior is removed
			break; // Exit loop after deletion to avoid iterator issues
		}

		// Show configuration when expanded
		if (isOpen) {
			ImGui::Indent();

			// Show the behavior's configuration
			ShowBehaviorConfiguration(node);

			// Special handling for BehaviorMultiConfig children
			if (node.name == "BehaviorMultiConfig") {
				ImGui::Separator();

				// Add button for child MultiConfig nodes
				if (ImGui::Button("Add Child Behavior")) {
					m_activeDropdownNodeId = node.id;
				}

				// Show dropdown for this child MultiConfig
				ShowBehaviorDropdown(node);

				// Render its children recursively
				if (!node.children.empty()) {
					ImGui::Separator();
					ImGui::Text("Child Behaviors:");
					RenderBehaviorPanels(node);
				}
			}

			ImGui::Unindent();
		}

		ImGui::PopID();
	}
}

void Tool::UI::UIMonsterProperties::AddBehaviorToNode(BehaviorNode& parent, const std::string& behaviorName)
{
	LOG_INFO("Add behavior called: " + behaviorName);
	auto newNode = CreateBehaviorNode(behaviorName);
	newNode->parent = &parent;
	parent.children.push_back(std::move(newNode));
	SaveCurrentProperties(); // Save when a new behavior is added
}

void Tool::UI::UIMonsterProperties::ShowBehaviorConfiguration(BehaviorNode& node)
{
	// Display appropriate configuration UI based on node type
	if (node.name == "BehaviorChase") {
		BehaviorChaseConfigUI(node);
	}
	else if (node.name == "BehaviorShootProjectile") {
		BehaviorShootProjectileConfigUI(node);
	}
	else if (node.name == "BehaviorMultiConfig") {
		BehaviorMultiConfigSettingsOnly(node);
	}
	else {
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
			"Unsupported behavior type: %s", node.name.c_str());
	}
}

void Tool::UI::UIMonsterProperties::BehaviorMultiConfigSettingsOnly(BehaviorNode& node)
{
	if (auto* multiConfig = dynamic_cast<BehaviorMultiConfig*>(node.config.get())) {
		ImGui::Text("Container Type:");
		static const char* containerTypes[] = {
			"SelectorWithRunning",
			"ProgressiveSequence",
			"Sequence",
			"Selector",
			"Parallel",
			"Race",
		};
		int containerType = static_cast<int>(multiConfig->containerType);
		if (ImGui::Combo("Type", &containerType, containerTypes, IM_ARRAYSIZE(containerTypes))) {
			multiConfig->containerType = static_cast<ContainerType>(containerType);
			SaveCurrentProperties(); // Save when container type changes
		}
	}
}

// Updated configuration methods to work with node data
void Tool::UI::UIMonsterProperties::BehaviorChaseConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorChaseConfig*>(node.config.get())) {
		bool changed = false;

		// Display the speed with a tooltip about the internal format
		if (ImGui::InputInt("Chase Speed", &config->chaseSpeed)) {
			changed = true;
		}

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Internal value (scaled by 10000)");
			ImGui::Text("Actual speed: %.2f", config->chaseSpeed / 10000.0f);
			ImGui::EndTooltip();
		}

		if (changed) {
			SaveCurrentProperties();
		}
	}
}

void Tool::UI::UIMonsterProperties::BehaviorShootProjectileConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorShootProjectileConfig*>(node.config.get())) {
		bool hasChanged = false;

		// CoolDown with tooltip
		if (ImGui::InputInt("CoolDown", &config->coolDown)) {
			hasChanged = true;
		}

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Internal value (scaled by 10000)");
			ImGui::Text("Actual cooldown: %.2f seconds", config->coolDown / 10000.0f);
			ImGui::EndTooltip();
		}

		// Bullet properties
		if (ImGui::CollapsingHeader("Bullet Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();

			// Valid bullet selection
			std::vector<const char*> validBulletsIngame;
			for (auto& [name, texture] : BulletTextureMap) {
				validBulletsIngame.push_back(name.c_str());
			}

			int currentBulletIndex = 0;
			for (int i = 0; i < validBulletsIngame.size(); i++) {
				if (config->bulletConfig.validBulletIngame == validBulletsIngame[i]) {
					currentBulletIndex = i;
					break;
				}
			}

			if (ImGui::Combo("Bullet Type", &currentBulletIndex, validBulletsIngame.data(),
				static_cast<int>(validBulletsIngame.size()))) {
				config->bulletConfig.validBulletIngame = validBulletsIngame[currentBulletIndex];
				hasChanged = true;
			}

			// Bullet properties
			if (ImGui::InputInt("Speed", &config->bulletConfig.speed) ||
				ImGui::InputInt("Alive Time", &config->bulletConfig.aliveTime) ||
				ImGui::InputInt("Damage", &config->bulletConfig.damage) ||
				ImGui::InputInt("Bounce", &config->bulletConfig.bounce)) {
				hasChanged = true;
			}

			ImGui::Unindent();
		}

		if (hasChanged) {
			SaveCurrentProperties();
		}
	}
}

// Helper function to recursively convert BehaviorConfig to BehaviorNode
std::unique_ptr<Tool::UI::BehaviorNode> Tool::UI::UIMonsterProperties::ConvertBehaviorConfigToNode(const BehaviorConfig* config)
{
	if (!config) return nullptr;

	// Create a node based on behavior type
	std::string behaviorType = config->GetBehaviorType();
	auto node = std::make_unique<BehaviorNode>(GetNextNodeId(), behaviorType);

	// Create a deep copy of the config for the node
	if (auto* chase = dynamic_cast<const BehaviorChaseConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorChaseConfig>();
		configCopy->chaseSpeed = chase->chaseSpeed;
		node->config = std::move(configCopy);
	}
	else if (auto* projectile = dynamic_cast<const BehaviorShootProjectileConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorShootProjectileConfig>();
		configCopy->coolDown = projectile->coolDown;
		configCopy->bulletConfig = projectile->bulletConfig;
		node->config = std::move(configCopy);
	}
	else if (auto* multi = dynamic_cast<const BehaviorMultiConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorMultiConfig>();
		configCopy->containerType = multi->containerType;
		node->config = std::move(configCopy);

		// Recursively convert child behaviors
		for (const auto& childConfig : multi->childBehaviors) {
			auto childNode = ConvertBehaviorConfigToNode(childConfig.get());
			if (childNode) {
				childNode->parent = node.get();
				node->children.push_back(std::move(childNode));
			}
		}
	}
	else {
		// Unsupported behavior type, log warning and create default config
		LOG_ERROR("Unsupported behavior type in ConvertBehaviorConfigToNode: " + behaviorType);
		node->config = std::make_unique<BehaviorMultiConfig>();
	}

	return node;
}

// Helper function to recursively convert BehaviorNode to BehaviorConfig
std::unique_ptr<BehaviorConfig> Tool::UI::UIMonsterProperties::ConvertNodeToBehaviorConfig(const BehaviorNode& node)
{
	if (!node.config) return nullptr;

	std::unique_ptr<BehaviorConfig> config;

	// Create appropriate config based on node's name
	if (node.name == "BehaviorChase") {
		if (auto* chase = dynamic_cast<const BehaviorChaseConfig*>(node.config.get())) {
			auto configCopy = std::make_unique<BehaviorChaseConfig>();
			configCopy->chaseSpeed = chase->chaseSpeed;
			config = std::move(configCopy);
		}
	}
	else if (node.name == "BehaviorShootProjectile") {
		if (auto* projectile = dynamic_cast<const BehaviorShootProjectileConfig*>(node.config.get())) {
			auto configCopy = std::make_unique<BehaviorShootProjectileConfig>();
			configCopy->coolDown = projectile->coolDown;
			configCopy->bulletConfig = projectile->bulletConfig;
			config = std::move(configCopy);
		}
	}
	else if (node.name == "BehaviorMultiConfig") {
		if (auto* multi = dynamic_cast<const BehaviorMultiConfig*>(node.config.get())) {
			auto configCopy = std::make_unique<BehaviorMultiConfig>();
			configCopy->containerType = multi->containerType;

			// Recursively convert child nodes
			for (const auto& childNode : node.children) {
				auto childConfig = ConvertNodeToBehaviorConfig(*childNode);
				if (childConfig) {
					configCopy->childBehaviors.push_back(std::move(childConfig));
				}
			}
			config = std::move(configCopy);
		}
	}
	else {
		// Unsupported behavior type, log warning and create default config
		LOG_ERROR("Unsupported behavior type in ConvertNodeToBehaviorConfig: " + node.name);
		config = std::make_unique<BehaviorMultiConfig>();
	}

	return config;
}

// Convert MonsterProperties to UI BehaviorNode tree
void Tool::UI::UIMonsterProperties::ConvertFromMonsterProperties(const MonsterTypeDefinition& properties)
{
	// Reset and initialize root node
	InitializeRootNode();

	if (!properties.defaultProperties.rootBehavior) {
		return; // Nothing to convert
	}

	// Convert the root behavior and its children
	auto rootConfig = dynamic_cast<const BehaviorMultiConfig*>(properties.defaultProperties.rootBehavior.get());
	if (rootConfig) {
		// Update root node config
		if (auto* multiConfig = dynamic_cast<BehaviorMultiConfig*>(m_pRootNode->config.get())) {
			multiConfig->containerType = rootConfig->containerType;
		}

		// Clear existing children and convert from properties
		m_pRootNode->children.clear();

		// Convert each child behavior from the properties
		for (const auto& childConfig : rootConfig->childBehaviors) {
			auto childNode = ConvertBehaviorConfigToNode(childConfig.get());
			if (childNode) {
				childNode->parent = m_pRootNode.get();
				m_pRootNode->children.push_back(std::move(childNode));
			}
		}
	}
}

// Convert UI BehaviorNode tree to MonsterProperties
void Tool::UI::UIMonsterProperties::ConvertToMonsterProperties(MonsterTypeDefinition& properties)
{
	if (!m_pRootNode) {
		// Initialize empty behavior tree
		properties.defaultProperties.rootBehavior = std::make_unique<BehaviorMultiConfig>();
		return;
	}

	// Create new root behavior
	auto newRootBehavior = std::make_unique<BehaviorMultiConfig>();

	// Copy container type from root node
	if (auto* rootMultiConfig = dynamic_cast<const BehaviorMultiConfig*>(m_pRootNode->config.get())) {
		newRootBehavior->containerType = rootMultiConfig->containerType;
	}

	// Convert all child nodes to behavior configs
	for (const auto& childNode : m_pRootNode->children) {
		auto childConfig = ConvertNodeToBehaviorConfig(*childNode);
		if (childConfig) {
			newRootBehavior->childBehaviors.push_back(std::move(childConfig));
		}
	}

	// Replace the properties' behavior tree
	properties.defaultProperties.rootBehavior = std::move(newRootBehavior);
}

void Tool::UI::UIMonsterProperties::SaveCurrentProperties()
{
	if (m_pCurrentProperties && m_hasValidSelection) {
		// Convert UI nodes back to monster properties
		ConvertToMonsterProperties(*m_pCurrentProperties);
		Core::EventData eventData;
		eventData.data = *m_pCurrentProperties;
		Core::EventSystem::getInstance().publish(EventKeys::MonsterUpdated, eventData);
	}
}

void Tool::UI::UIMonsterProperties::ShowBehaviorDropdown(BehaviorNode& node)
{
	// Only show dropdown if this node is the active one
	if (m_activeDropdownNodeId != node.id) {
		return;
	}

	// Position dropdown below the button
	ImVec2 buttonPos = ImGui::GetItemRectMin();
	ImVec2 buttonSize = ImGui::GetItemRectSize();

	ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + buttonSize.y));
	ImGui::SetNextWindowSize(ImVec2(250.0f, 200.0f));

	// Create unique window name using node ID
	std::string windowName = "##BehaviorDropdown" + std::to_string(node.id);
	bool isOpen = (m_activeDropdownNodeId == node.id);

	if (ImGui::Begin(windowName.c_str(), &isOpen,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::Text("Select Behavior:");
		ImGui::Separator();

		if (ImGui::BeginChild("BehaviorList", ImVec2(0, 150.0f), true))
		{
			// Only show the three supported behavior types
			const char* supportedBehaviors[] = {
				"BehaviorMultiConfig",
				"BehaviorChase",
				"BehaviorShootProjectile"
			};

			for (int n = 0; n < IM_ARRAYSIZE(supportedBehaviors); n++) {
				// Add a more user-friendly display name
				const char* displayName;
				if (strcmp(supportedBehaviors[n], "BehaviorMultiConfig") == 0)
					displayName = "Multi Behavior";
				else if (strcmp(supportedBehaviors[n], "BehaviorChase") == 0)
					displayName = "Chase";
				else if (strcmp(supportedBehaviors[n], "BehaviorShootProjectile") == 0)
					displayName = "Shoot Projectile";
				else
					displayName = supportedBehaviors[n];

				if (ImGui::Selectable(displayName, false)) {
					// Empty - just for visual feedback
				}

				if (ImGui::IsItemClicked())
				{
					AddBehaviorToNode(node, supportedBehaviors[n]);
					m_activeDropdownNodeId = -1; // Close dropdown
					break;
				}

				// Optional: Add hover effect or tooltip
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click to add: %s", displayName);
					ImGui::EndTooltip();
				}
			}
		}
		ImGui::EndChild();

		// Cancel button
		if (ImGui::Button("Cancel", ImVec2(-1, 0))) {
			m_activeDropdownNodeId = -1;
		}
	}
	ImGui::End();

	// Close dropdown if window was closed or clicked outside
	if (!isOpen || (!ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))) {
		m_activeDropdownNodeId = -1;
	}
}

// Create a behavior config for a behavior type
std::unique_ptr<BehaviorConfig> Tool::UI::UIMonsterProperties::CreateBehaviorConfig(const std::string& behaviorType)
{
	// Use factory method if possible
	auto config = BehaviorFactory::GetInstance().CreateBehavior(behaviorType);
	if (config) return config;

	// Fallback to direct creation for the three supported types
	if (behaviorType == "BehaviorChase") {
		return std::make_unique<BehaviorChaseConfig>();
	}
	else if (behaviorType == "BehaviorShootProjectile") {
		return std::make_unique<BehaviorShootProjectileConfig>();
	}
	else if (behaviorType == "BehaviorMultiConfig") {
		return std::make_unique<BehaviorMultiConfig>();
	}

	// Return MultiConfig as fallback for unsupported types
	LOG_ERROR("Creating default MultiConfig for unsupported behavior type: " + behaviorType);
	return std::make_unique<BehaviorMultiConfig>();
}