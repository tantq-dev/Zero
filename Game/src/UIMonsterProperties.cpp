#include "UIMonsterProperties.h"
#include <imgui.h>
#include <vector>
#include <string>

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

std::unique_ptr<Tool::UI::BehaviorNode> Tool::UI::UIMonsterProperties::CreateBehaviorNode(const std::string& behaviorName)
{
	auto node = std::make_unique<BehaviorNode>(GetNextNodeId(), behaviorName);

	// Create appropriate config based on behavior name
	if (behaviorName == "BehaviorChase") {
		node->config = std::make_unique<BehaviorChaseConfig>();
	}
	else if (behaviorName == "BehaviorDistanceConditionHelper") {
		node->config = std::make_unique<BehaviorDistanceConditionHelperConfig>();
	}
	else if (behaviorName == "BehaviorMovementBounce") {
		node->config = std::make_unique<BehaviorMovementBounceConfig>();
	}
	else if (behaviorName == "BehaviorShootBarrage") {
		node->config = std::make_unique<BehaviorShootBarrageConfig>();
	}
	else if (behaviorName == "BehaviorShootProjectile") {
		node->config = std::make_unique<BehaviorShootProjectileConfig>();
	}
	else if (behaviorName == "BehaviorShootStrategyBase") {
		node->config = std::make_unique<BehaviorShootStrategyBaseConfig>();
	}
	else if (behaviorName == "BehaviorSpreadShot") {
		node->config = std::make_unique<BehaviorSpreadShotConfig>();
	}
	else if (behaviorName == "BehaviorMultiConfig") {
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
				const char* monsterType[] = { "Boss", "Normal" };
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

void Tool::UI::UIMonsterProperties::SetCurrentProperties(MonsterTypeDefinition& properties)
{
	m_pCurrentProperties = &properties;
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
			"Sequence"
		};
		int containerType = static_cast<int>(multiConfig->containerType);
		if (ImGui::Combo("Type", &containerType, containerTypes, IM_ARRAYSIZE(containerTypes))) {
			multiConfig->containerType = static_cast<ContainerType>(containerType);
		}
		ImGui::Separator();
	}

	// Add new behavior to root
	ImGui::Text("Add New Behavior:");
	if (ImGui::BeginCombo("Behavior Type", availableItems[currentSelection])) {
		for (int n = 0; n < IM_ARRAYSIZE(availableItems); n++) {
			bool isSelected = (currentSelection == n);
			if (ImGui::Selectable(availableItems[n], isSelected)) {
				currentSelection = n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Add Behavior")) {
		AddBehaviorToNode(*m_pRootNode, availableItems[currentSelection]);
	}

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
			ShowBehaviorConfiguration(node);

			// If this behavior has children (like nested BehaviorMultiConfig), render them recursively
			if (!node.children.empty()) {
				ImGui::Separator();
				ImGui::Text("Child Behaviors:");
				RenderBehaviorPanels(node);
			}

			ImGui::Unindent();
		}

		ImGui::PopID();
	}
}

void Tool::UI::UIMonsterProperties::AddBehaviorToNode(BehaviorNode& parent, const std::string& behaviorName)
{
	auto newNode = CreateBehaviorNode(behaviorName);
	newNode->parent = &parent;
	parent.children.push_back(std::move(newNode));
}

void Tool::UI::UIMonsterProperties::ShowBehaviorConfiguration(BehaviorNode& node)
{
	// Display appropriate configuration UI based on node type
	if (node.name == "BehaviorChase") {
		BehaviorChaseConfigUI(node);
	}
	else if (node.name == "BehaviorDistanceConditionHelper") {
		BehaviorDistanceConditionHelperConfigUI(node);
	}
	else if (node.name == "BehaviorMovementBounce") {
		BehaviorMovementBounceConfigUI(node);
	}
	else if (node.name == "BehaviorShootBarrage") {
		BehaviorShootBarrageConfigUI(node);
	}
	else if (node.name == "BehaviorShootProjectile") {
		BehaviorShootProjectileConfigUI(node);
	}
	else if (node.name == "BehaviorShootStrategyBase") {
		BehaviorShootStrategyBaseConfigUI(node);
	}
	else if (node.name == "BehaviorSpreadShot") {
		BehaviorSpreadShotConfigUI(node);
	}
	else if (node.name == "BehaviorMultiConfig") {
		BehaviorMultiConfigUI(node);
	}
	else {
		ImGui::Text("No configuration available for this behavior type.");
	}
}

// Updated configuration methods to work with node data
void Tool::UI::UIMonsterProperties::BehaviorChaseConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorChaseConfig*>(node.config.get())) {
		ImGui::InputInt("Chase Speed", &config->chaseSpeed);
	}
}

void Tool::UI::UIMonsterProperties::BehaviorShootBarrageConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorShootBarrageConfig*>(node.config.get())) {
		ImGui::InputInt("CoolDown", &config->coolDown);

		const char* bulletTypeNames[] = { "straight", "parabol", "mortal", "boss" };
		int bulletType = static_cast<int>(config->bulletConfig.bulletType);
		if (ImGui::Combo("Bullet Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
			config->bulletConfig.bulletType = static_cast<BulletType>(bulletType);
		}

		ImGui::InputInt("NumOfBullet", &config->numOfBullet);
		ImGui::InputInt("SpreadAngle", &config->spreadAngle);
		ImGui::InputInt("Speed", &config->bulletConfig.speed);
		ImGui::InputInt("AliveTime", &config->bulletConfig.aliveTime);
		ImGui::InputInt("Damage", &config->bulletConfig.damage);
		ImGui::InputInt("Bounce", &config->bulletConfig.bounce);



	}
}

void Tool::UI::UIMonsterProperties::BehaviorMultiConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorMultiConfig*>(node.config.get())) {
		// Container type selection
		const char* containerTypes[] = {
			"SelectorWithRunning",
			"ProgressiveSequence",
			"Sequence"
		};
		int containerType = static_cast<int>(config->containerType);
		if (ImGui::Combo("Container Type", &containerType, containerTypes, IM_ARRAYSIZE(containerTypes))) {
			config->containerType = static_cast<ContainerType>(containerType);
		}

		ImGui::Separator();

		// Add child behavior option
		ImGui::Text("Add Child Behavior:");
		if (ImGui::BeginCombo("Child Behavior Type", availableItems[currentSelection])) {
			for (int n = 0; n < IM_ARRAYSIZE(availableItems); n++) {
				bool isSelected = (currentSelection == n);
				if (ImGui::Selectable(availableItems[n], isSelected)) {
					currentSelection = n;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Add Child Behavior")) {
			AddBehaviorToNode(node, availableItems[currentSelection]);
		}
	}
}

// Implement other configuration UI methods similarly...
void Tool::UI::UIMonsterProperties::BehaviorDistanceConditionHelperConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorDistanceConditionHelperConfig*>(node.config.get())) {
		ImGui::InputInt("Max distance", &config->maxDistance);
		ImGui::InputInt("Min distance", &config->minDistance);
	}
}

void Tool::UI::UIMonsterProperties::BehaviorMovementBounceConfigUI(BehaviorNode& node)
{
	// No configuration for this behavior
	ImGui::Text("No configuration available for Movement Bounce.");
}

void Tool::UI::UIMonsterProperties::BehaviorShootProjectileConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorShootProjectileConfig*>(node.config.get())) {
		ImGui::InputInt("CoolDown", &config->coolDown);

		const char* bulletTypeNames[] = { "straight", "parabol", "mortal", "boss" };
		int bulletType = static_cast<int>(config->bulletConfig.bulletType);
		if (ImGui::Combo("Bullet Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
			config->bulletConfig.bulletType = static_cast<BulletType>(bulletType);
		}
		ImGui::InputInt("Speed", &config->bulletConfig.speed);
		ImGui::InputInt("AliveTime", &config->bulletConfig.aliveTime);
		ImGui::InputInt("Damage", &config->bulletConfig.damage);
		ImGui::InputInt("Bounce", &config->bulletConfig.bounce);
	}
}



void Tool::UI::UIMonsterProperties::BehaviorShootStrategyBaseConfigUI(BehaviorNode& node)
{
	// No configuration for this behavior
	ImGui::Text("No configuration available for Shoot Strategy Base.");
}

void Tool::UI::UIMonsterProperties::BehaviorSpreadShotConfigUI(BehaviorNode& node)
{
	if (auto* config = dynamic_cast<BehaviorSpreadShotConfig*>(node.config.get())) {
		ImGui::InputInt("CoolDown", &config->coolDown);

		const char* bulletTypeNames[] = { "straight", "parabol", "mortal", "boss" };
		int bulletType = static_cast<int>(config->bulletConfig.bulletType);
		if (ImGui::Combo("Bullet Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
			config->bulletConfig.bulletType = static_cast<BulletType>(bulletType);
		}

		ImGui::InputInt("NumOfBullet", &config->numOfBullet);
		ImGui::InputInt("SpreadAngle", &config->spreadAngle);

		ImGui::InputInt("Speed", &config->bulletConfig.speed);
		ImGui::InputInt("AliveTime", &config->bulletConfig.aliveTime);
		ImGui::InputInt("Damage", &config->bulletConfig.damage);
		ImGui::InputInt("Bounce", &config->bulletConfig.bounce);
	}
}

// Helper function to recursively convert BehaviorConfig to BehaviorNode
std::unique_ptr<Tool::UI::BehaviorNode> Tool::UI::UIMonsterProperties::ConvertBehaviorConfigToNode(const BehaviorConfig* config)
{
	if (!config) return nullptr;

	auto node = std::make_unique<BehaviorNode>(GetNextNodeId(), config->behaviorType);

	// Create a deep copy of the config for the node
	if (auto* chase = dynamic_cast<const BehaviorChaseConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorChaseConfig>();
		configCopy->chaseSpeed = chase->chaseSpeed;
		node->config = std::move(configCopy);
	}
	else if (auto* distance = dynamic_cast<const BehaviorDistanceConditionHelperConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorDistanceConditionHelperConfig>();
		configCopy->maxDistance = distance->maxDistance;
		configCopy->minDistance = distance->minDistance;
		node->config = std::move(configCopy);
	}
	else if (auto* bounce = dynamic_cast<const BehaviorMovementBounceConfig*>(config)) {
		node->config = std::make_unique<BehaviorMovementBounceConfig>();
	}
	else if (auto* barrage = dynamic_cast<const BehaviorShootBarrageConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorShootBarrageConfig>();
		configCopy->coolDown = barrage->coolDown;
		configCopy->bulletConfig = barrage->bulletConfig;
		configCopy->numOfBullet = barrage->numOfBullet;
		configCopy->spreadAngle = barrage->spreadAngle;
		node->config = std::move(configCopy);
	}
	else if (auto* projectile = dynamic_cast<const BehaviorShootProjectileConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorShootProjectileConfig>();
		configCopy->coolDown = projectile->coolDown;
		configCopy->bulletConfig = projectile->bulletConfig;
		node->config = std::move(configCopy);
	}
	else if (auto* strategy = dynamic_cast<const BehaviorShootStrategyBaseConfig*>(config)) {
		node->config = std::make_unique<BehaviorShootStrategyBaseConfig>();
	}
	else if (auto* spread = dynamic_cast<const BehaviorSpreadShotConfig*>(config)) {
		auto configCopy = std::make_unique<BehaviorSpreadShotConfig>();
		configCopy->coolDown = spread->coolDown;
		configCopy->bulletConfig = spread->bulletConfig;
		configCopy->numOfBullet = spread->numOfBullet;
		configCopy->spreadAngle = spread->spreadAngle;
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
		// Unknown behavior type, create default config
		node->config = CreateBehaviorConfig(config->behaviorType);
	}

	return node;
}

// Helper function to recursively convert BehaviorNode to BehaviorConfig
std::unique_ptr<BehaviorConfig> Tool::UI::UIMonsterProperties::ConvertNodeToBehaviorConfig(const BehaviorNode& node)
{
	if (!node.config) return nullptr;

	std::unique_ptr<BehaviorConfig> config;

	// Create appropriate config based on node's config type
	if (auto* chase = dynamic_cast<const BehaviorChaseConfig*>(node.config.get())) {
		auto configCopy = std::make_unique<BehaviorChaseConfig>();
		configCopy->chaseSpeed = chase->chaseSpeed;
		config = std::move(configCopy);
	}
	else if (auto* distance = dynamic_cast<const BehaviorDistanceConditionHelperConfig*>(node.config.get())) {
		auto configCopy = std::make_unique<BehaviorDistanceConditionHelperConfig>();
		configCopy->maxDistance = distance->maxDistance;
		configCopy->minDistance = distance->minDistance;
		config = std::move(configCopy);
	}
	else if (auto* bounce = dynamic_cast<const BehaviorMovementBounceConfig*>(node.config.get())) {
		config = std::make_unique<BehaviorMovementBounceConfig>();
	}
	else if (auto* barrage = dynamic_cast<const BehaviorShootBarrageConfig*>(node.config.get())) {
		auto configCopy = std::make_unique<BehaviorShootBarrageConfig>();
		configCopy->coolDown = barrage->coolDown;
		configCopy->bulletConfig = barrage->bulletConfig;
		configCopy->numOfBullet = barrage->numOfBullet;
		configCopy->spreadAngle = barrage->spreadAngle;
		config = std::move(configCopy);
	}
	else if (auto* projectile = dynamic_cast<const BehaviorShootProjectileConfig*>(node.config.get())) {
		auto configCopy = std::make_unique<BehaviorShootProjectileConfig>();
		configCopy->coolDown = projectile->coolDown;
		configCopy->bulletConfig = projectile->bulletConfig;
		config = std::move(configCopy);
	}
	else if (auto* strategy = dynamic_cast<const BehaviorShootStrategyBaseConfig*>(node.config.get())) {
		config = std::make_unique<BehaviorShootStrategyBaseConfig>();
	}
	else if (auto* spread = dynamic_cast<const BehaviorSpreadShotConfig*>(node.config.get())) {
		auto configCopy = std::make_unique<BehaviorSpreadShotConfig>();
		configCopy->coolDown = spread->coolDown;
		configCopy->bulletConfig = spread->bulletConfig;
		configCopy->numOfBullet = spread->numOfBullet;
		configCopy->spreadAngle = spread->spreadAngle;
		config = std::move(configCopy);
	}
	else if (auto* multi = dynamic_cast<const BehaviorMultiConfig*>(node.config.get())) {
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
	else {
		// Fallback: create config using factory
		config = CreateBehaviorConfig(node.name);
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
	}
}