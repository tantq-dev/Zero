#include "UIMonsterProperties.h"
#include <imgui.h>
#include <vector>
#include <string>
#include "EventKey.h"
#include "core/EventSystem.h"
#include "utilities/Logger.h"

Tool::UI::UIMonsterProperties::UIMonsterProperties()
{

}

Tool::UI::UIMonsterProperties::~UIMonsterProperties()
{

}

void Tool::UI::UIMonsterProperties::InitializeRootNode()
{
	m_pRootNode = std::make_unique<BehaviorNode>(GetNextNodeId(), "BehaviorMultipleConfig");
	m_pRootNode->config = std::make_unique<BehaviorMultiConfig>();
}

std::unique_ptr<Tool::UI::BehaviorNode> Tool::UI::UIMonsterProperties::CreateBehaviorNode(const BehaviorType type)
{
	// Create a new node with the specified behavior name
	auto node = std::make_unique<BehaviorNode>(GetNextNodeId(), BehaviorTypeToString(type));

	// Use the factory function to create the appropriate config
	node->config = CreateBehaviorConfig(type);

	// If the factory function failed to create a config, add some error handling
	if (!node->config) {
		LOG_INFO("Failed to create config for behavior type: " + BehaviorTypeToString(type));
		// Could set a default config or leave it null
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

				std::vector<std::string> bulletTypeStrings = GetMonstersTypeString();
				const char* monsterType[static_cast<int>(MonsterType::COUNT)] = {};

				for (size_t i = 0; i < bulletTypeStrings.size(); i++)
				{
					monsterType[i] = bulletTypeStrings[i].c_str();
				}

				// Monster type
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
				if (ImGui::InputFloat("Speed", &m_pCurrentProperties->defaultProperties.speed)) {
					dataChanged = true;
				}

				// Knockback resistance
				if (ImGui::InputFloat("Knockback resistance", &m_pCurrentProperties->defaultProperties.knockbackResistance)) {
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

		std::vector<std::string> containerTypeStrings = GetContainerTypesString();
		const char* containerTypes[static_cast<int>(ContainerType::COUNT)] = {};

		for (size_t i = 0; i < containerTypeStrings.size(); i++)
		{
			containerTypes[i] = containerTypeStrings[i].c_str();
		}
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
			break; // Exit loop after deletion to avoid iterator issues
		}

		// Show configuration when expanded
		if (isOpen) {
			ImGui::Indent();

			// Show the behavior's configuration
			ShowBehaviorConfiguration(node);

			// Special handling for BehaviorMultiConfig children
			if (node.name == "MultiConfig") {
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
			// For other behavior types, still render children if they exist
			else if (!node.children.empty()) {
				ImGui::Separator();
				ImGui::Text("Child Behaviors:");
				RenderBehaviorPanels(node);
			}

			ImGui::Unindent();
		}

		ImGui::PopID();
	}
}

void Tool::UI::UIMonsterProperties::AddBehaviorToNode(BehaviorNode& parent, const BehaviorType type)
{
	auto newNode = CreateBehaviorNode(type);
	newNode->parent = &parent;
	parent.children.push_back(std::move(newNode));
}

void Tool::UI::UIMonsterProperties::ShowBehaviorConfiguration(BehaviorNode& node)
{
	if (!node.config) {
		ImGui::Text("No configuration available for this behavior type.");
		return;
	}

	// Get the configuration fields from the behavior config
	std::vector<ConfigField> fields = node.config->GetConfigFields();
	// Render the fields
	bool changed = RenderConfigFields(fields);
	// If something changed, save the properties
	if (changed) {
		SaveCurrentProperties();
	}
}


bool Tool::UI::UIMonsterProperties::RenderConfigFields(const std::vector<ConfigField>& fields)
{
	bool changed = false;

	for (const auto& field : fields) {
		switch (field.type) {
		case ConfigFieldType::Int: {
			int* value = static_cast<int*>(field.valuePtr);
			if (ImGui::InputInt(field.name.c_str(), value)) {
				changed = true;
				if (field.onChange) {
					changed |= field.onChange(field.valuePtr);
				}
			}
			break;
		}
		case ConfigFieldType::Combo: {
			int* value = static_cast<int*>(field.valuePtr);
			std::vector<const char*> options;
			for (const auto& option : field.options) {
				options.push_back(option.c_str());
			}

			if (ImGui::Combo(field.name.c_str(), value, options.data(), static_cast<int>(options.size()))) {
				changed = true;
				if (field.onChange) {
					changed |= field.onChange(field.valuePtr);
				}
			}
			break;
		}
		case ConfigFieldType::Text: {
			std::string* value = static_cast<std::string*>(field.valuePtr);
			char buffer[256];
			strncpy(buffer, value->c_str(), sizeof(buffer) - 1);
			buffer[sizeof(buffer) - 1] = '\0';

			if (ImGui::InputText(field.name.c_str(), buffer, sizeof(buffer))) {
				*value = buffer;
				changed = true;
				if (field.onChange) {
					changed |= field.onChange(field.valuePtr);
				}
			}
			break;
		}
		case ConfigFieldType::Bullet: {
			BulletConfig* bulletConfig = static_cast<BulletConfig*>(field.valuePtr);
			if (ImGui::TreeNode(field.name.c_str())) {
				changed |= RenderBulletConfigFields(bulletConfig);
				ImGui::TreePop();
			}
			break;
		}
		case ConfigFieldType::None:
		default:
			break;
		}
	}

	return changed;
}

bool Tool::UI::UIMonsterProperties::RenderBulletConfigFields(BulletConfig* bulletConfig)
{
	bool changed = false;

	// Get available bullets
	std::vector<const char*> validBulletsIngame = GetAvailableBullets();

	// Find current bullet index
	int currentBulletIndex = 0;
	for (int i = 0; i < validBulletsIngame.size(); i++) {
		if (bulletConfig->validBulletIngame == validBulletsIngame[i]) {
			currentBulletIndex = i;
			break;
		}
	}

	// Bullet type dropdown
	if (ImGui::Combo("Valid Bullet Ingame", &currentBulletIndex, validBulletsIngame.data(), static_cast<int>(validBulletsIngame.size()))) {
		bulletConfig->validBulletIngame = validBulletsIngame[currentBulletIndex];
		changed = true;
	}

	// Bullet type
	std::vector<std::string> bulletTypeString = GetBulletsTypeString();
	const char* bulletTypeNames[static_cast<int>(BulletType::COUNT)] = {};

	for (size_t i = 0; i < bulletTypeString.size(); i++)
	{
		bulletTypeNames[i] = bulletTypeString[i].c_str();
	}

	int bulletType = static_cast<int>(bulletConfig->bulletType);
	if (ImGui::Combo("Bullet Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
		bulletConfig->bulletType = static_cast<BulletType>(bulletType);
		changed = true;
	}

	// Other bullet properties
	if (ImGui::InputInt("Speed", &bulletConfig->speed) ||
		ImGui::InputInt("AliveTime", &bulletConfig->aliveTime) ||
		ImGui::InputInt("Damage", &bulletConfig->damage) ||
		ImGui::InputInt("Bounce", &bulletConfig->bounce)) {
		changed = true;
	}

	return changed;
}

std::vector<const char*> Tool::UI::UIMonsterProperties::GetAvailableBullets() const
{
	std::vector<const char*> validBulletsIngame;

	for (auto& [name, texture] : BulletTextureMap) {
		validBulletsIngame.push_back(name.c_str());
	}

	return validBulletsIngame;
}

void Tool::UI::UIMonsterProperties::BehaviorMultiConfigSettingsOnly(BehaviorNode& node)
{
	// Container type configuration for root
	if (auto* multiConfig = dynamic_cast<BehaviorMultiConfig*>(node.config.get())) {
		ImGui::Text("Container Type:");

		std::vector<std::string> containerTypeStrings = GetContainerTypesString();
		const char* containerTypes[static_cast<int>(ContainerType::COUNT)] = {};

		for (size_t i = 0; i < containerTypeStrings.size(); i++)
		{
			containerTypes[i] = containerTypeStrings[i].c_str();
		}
		int containerType = static_cast<int>(multiConfig->containerType);

		if (ImGui::Combo("Type", &containerType, containerTypes, IM_ARRAYSIZE(containerTypes))) {
			multiConfig->containerType = static_cast<ContainerType>(containerType);
			SaveCurrentProperties();

		}
		ImGui::Separator();
	}

	// Add new behavior to root
	if (ImGui::Button("Add Behavior")) {
		m_activeDropdownNodeId = node.id;
	}

	// Show behavior selection dropdown for this specific node
	ShowBehaviorDropdown(node);

	ImGui::Separator();

	// Display behaviors as expandable panels
	ImGui::Text("Behaviors:");
	RenderBehaviorPanels(node);
}

// Helper function to recursively convert BehaviorConfig to BehaviorNode
std::unique_ptr<Tool::UI::BehaviorNode> Tool::UI::UIMonsterProperties::ConvertBehaviorConfigToNode(const BehaviorConfig* config)
{
	if (!config) return nullptr;

	// Create a new node with the proper behavior type
	auto node = std::make_unique<BehaviorNode>(GetNextNodeId(), config->behaviorType);

	// Use clone to create a deep copy of the config
	node->config = config->clone();

	// Special handling for BehaviorMultiConfig to recursively process children
	if (auto* multiConfig = dynamic_cast<const BehaviorMultiConfig*>(config)) {
		// Recursively convert child behaviors
		for (const auto& childConfig : multiConfig->childBehaviors) {
			auto childNode = ConvertBehaviorConfigToNode(childConfig.get());
			if (childNode) {
				childNode->parent = node.get();
				node->children.push_back(std::move(childNode));
			}
		}
	}

	return node;
}

// Helper function to recursively convert BehaviorNode to BehaviorConfig
std::unique_ptr<BehaviorConfig> Tool::UI::UIMonsterProperties::ConvertNodeToBehaviorConfig(const BehaviorNode& node)
{
	if (!node.config) return nullptr;

	// Use the config's clone method to create a copy
	std::unique_ptr<BehaviorConfig> config = node.config->clone();

	// Special handling for BehaviorMultiConfig to recursively handle children
	if (auto* multiConfig = dynamic_cast<BehaviorMultiConfig*>(config.get())) {
		// Clear any existing children that might have been copied
		multiConfig->childBehaviors.clear();

		// Recursively convert child nodes
		for (const auto& childNode : node.children) {
			auto childConfig = ConvertNodeToBehaviorConfig(*childNode);
			if (childConfig) {
				multiConfig->childBehaviors.push_back(std::move(childConfig));
			}
		}
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
			for (int n = 0; n < availableBehavior.size(); n++) {
				if (ImGui::Selectable(BehaviorTypeToString(availableBehavior[n]).c_str(), false)) {
					// Empty - just for visual feedback
				}

				if (ImGui::IsItemClicked())
				{
					AddBehaviorToNode(node, availableBehavior[n]);
					m_activeDropdownNodeId = -1; // Close dropdown
					break;
				}

				// Optional: Add hover effect or tooltip
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Click to add: %s", BehaviorTypeToString(availableBehavior[n]).c_str());
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