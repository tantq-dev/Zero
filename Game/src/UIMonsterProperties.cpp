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

void Tool::UI::UIMonsterProperties::ShowUIMonsterProperties(bool* p_open)
{
	if (p_open) {
		bool window_contents_visible = ImGui::Begin("Monster Properties", p_open, ImGuiWindowFlags_MenuBar);
		if (!window_contents_visible) {
			ImGui::End();
			return;
		}

		// Load current monster properties at the start
		LoadCurrentMonsterProperties();

		if (m_hasValidSelection) {
			if (ImGui::TreeNode("Stat")) {
				// Bind UI controls to m_currentProperties

				// Name
				char nameBuf[128];
				strncpy(nameBuf, m_currentProperties.name.c_str(), sizeof(nameBuf));
				nameBuf[sizeof(nameBuf) - 1] = '\0';
				if (ImGui::InputText("Name: ", nameBuf, IM_ARRAYSIZE(nameBuf))) {
					m_currentProperties.name = nameBuf;
				}

				// Monster type
				const char* monsterType[] = { "Boss", "Normal" };
				int itemType = static_cast<int>(m_currentProperties.monsterType);
				if (ImGui::Combo("Monster Type", &itemType, monsterType, IM_ARRAYSIZE(monsterType))) {
					m_currentProperties.monsterType = static_cast<Data::MonsterType>(itemType);
				}

				// HP
				ImGui::InputInt("HP", &m_currentProperties.hp);

				// Move speed
				ImGui::InputInt("Speed", &m_currentProperties.speed);

				// Knockback resistance
				ImGui::InputInt("Knockback resistance", &m_currentProperties.knockbackResistance);

				// Collision damage
				ImGui::InputInt("Collision damage", &m_currentProperties.collisionDamage);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Behavior")) {
				BehaviorMultipleConfig();
				ImGui::TreePop();
			}

			// Save button
			if (ImGui::Button("Save Properties")) {
				SaveCurrentMonsterProperties();
			}
		}
		else {
			ImGui::Text("No monster selected. Select a monster to edit its properties.");
		}

		ImGui::End();
	}
}

void Tool::UI::UIMonsterProperties::LoadCurrentMonsterProperties()
{
	if (m_mapEditor) {
		MonsterProperties* properties = m_mapEditor->GetSelectedMonsterProperties();
		if (properties) {
			m_currentProperties = *properties;
			m_hasValidSelection = true;
			ConvertFromMonsterProperties(m_currentProperties);
		}
		else {
			m_hasValidSelection = false;
		}
	}
}

void Tool::UI::UIMonsterProperties::SaveCurrentMonsterProperties()
{
	if (m_mapEditor && m_hasValidSelection) {
		ConvertToMonsterProperties(m_currentProperties);
		m_mapEditor->UpdateMonsterPropertiesFromUI(m_currentProperties);
	}
}



void Tool::UI::UIMonsterProperties::RenderBehaviorTree(BehaviorNode& node) {
	if (node.children.empty()) {
		ImGui::BulletText("%s", node.name.c_str());
		return;
	}

	if (ImGui::TreeNode(node.name.c_str())) {
		for (auto& child : node.children) {
			RenderBehaviorTree(child);
		}
		ImGui::TreePop();
	}
}

void Tool::UI::UIMonsterProperties::RenderSelectableTree(BehaviorNode& node, BehaviorNode*& selectedNode) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	// Set selected flag if this is the selected node
	if (&node == selectedNode)
		flags |= ImGuiTreeNodeFlags_Selected;

	// Use leaf flag for nodes without children
	if (node.children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	bool isOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

	// Handle selection
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		selectedNode = &node;

	if (isOpen) {
		for (auto& child : node.children) {
			RenderSelectableTree(child, selectedNode);
		}
		ImGui::TreePop();
	}
}

void Tool::UI::UIMonsterProperties::BehaviorChaseConfigUI(BehaviorChaseConfig* config)
{
	static int chaseSpeed = 10000;
	if (config) {
		chaseSpeed = config->chaseSpeed;
	}

	if (ImGui::InputInt("Chase Speed", &chaseSpeed)) {
		if (config) {
			config->chaseSpeed = chaseSpeed;
		}
	}
}

void Tool::UI::UIMonsterProperties::BehaviorDistanceConditionHelperConfigUI()
{
	static int maxDistance = 10000;
	ImGui::InputInt("Max distance", &maxDistance);

	static int minDistance = 10000;
	ImGui::InputInt("Min distance", &minDistance);
}

void Tool::UI::UIMonsterProperties::BehaviorMovementBounceConfig()
{

}

void Tool::UI::UIMonsterProperties::BehaviorShootBarrageConfigUI(BehaviorShootBarrageConfig* config)
{
	static int coolDown = 10000;
	static int bulletType = 1;
	static int numOfBullet = 10000;
	static int spreadAngle = 10000;

	if (config) {
		coolDown = config->coolDown;
		bulletType = static_cast<int>(config->bulletType);
		numOfBullet = config->numOfBullet;
		spreadAngle = config->spreadAngle;
	}

	if (ImGui::InputInt("CoolDown", &coolDown)) {
		if (config) config->coolDown = coolDown;
	}

	const char* bulletTypeNames[] = { "straight", "parabol", "mortal", "boss" };
	if (ImGui::Combo("Bullet Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
		if (config) config->bulletType = static_cast<Data::BulletType>(bulletType);
	}

	if (ImGui::InputInt("NumOfBullet", &numOfBullet)) {
		if (config) config->numOfBullet = numOfBullet;
	}

	if (ImGui::InputInt("SpreadAngle", &spreadAngle)) {
		if (config) config->spreadAngle = spreadAngle;
	}
}


void Tool::UI::UIMonsterProperties::BehaviorShootProjectileConfig()
{
	static int coolDown = 10000;
	ImGui::InputInt("CoolDown", &coolDown);

	const char* bulletType[] =
	{
		"straight", "parabol", "mortal", "boss"
	};
	static int itemType = 1;
	ImGui::Combo("Bullet Type", &itemType, bulletType, IM_ARRAYSIZE(bulletType), IM_ARRAYSIZE(bulletType));
}

void Tool::UI::UIMonsterProperties::BehaviorShootStrategyBaseConfig()
{

}

void Tool::UI::UIMonsterProperties::BehaviorSpreadShotConfig()
{
	static int coolDown = 10000;
	ImGui::InputInt("CoolDown", &coolDown);

	const char* bulletType[] =
	{
		"straight", "parabol", "mortal", "boss"
	};
	static int itemType = 1;
	ImGui::Combo("Bullet Type", &itemType, bulletType, IM_ARRAYSIZE(bulletType), IM_ARRAYSIZE(bulletType));

	static int numOfBullet = 10000;
	ImGui::InputInt("NumOfBullet", &numOfBullet);

	static int spreadAngle = 10000;
	ImGui::InputInt("SpreadAngle", &spreadAngle);
}

void Tool::UI::UIMonsterProperties::BehaviorMultipleConfig()
{
	// Initialize root node as BehaviorMultipleConfig if not already set
	rootNode.name = "BehaviorMultipleConfig";

	static BehaviorNode* selectedNode = &rootNode;

	// Container type configuration (only for root BehaviorMultipleConfig)
	if (selectedNode == &rootNode) {
		ImGui::Text("Container Type:");
		static const char* containerTypes[] = {
			"SelectorWithRunning",
			"ProgressiveSequence",
			"Sequence"
		};
		static int containerType = 0;
		ImGui::Combo("Type", &containerType, containerTypes, IM_ARRAYSIZE(containerTypes));
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
		AddBehaviorToNode(rootNode, availableItems[currentSelection]);
	}

	ImGui::Separator();

	// Display behaviors as expandable panels
	ImGui::Text("Behaviors:");
	RenderBehaviorPanels(rootNode);
}

void Tool::UI::UIMonsterProperties::RenderBehaviorPanels(BehaviorNode& parentNode)
{
	for (int i = 0; i < parentNode.children.size(); i++) {
		BehaviorNode& node = parentNode.children[i];

		ImGui::PushID(i);

		// Create collapsing header for each behavior
		bool isOpen = ImGui::CollapsingHeader(node.name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);

		// Delete button on the same line
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
		if (ImGui::Button("X")) {
			parentNode.children.erase(parentNode.children.begin() + i);
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



void Tool::UI::UIMonsterProperties::AddBehaviorToNode(BehaviorNode& parent, const std::string& behaviorName) {
	// Create new node with the behavior name
	BehaviorNode newNode{ behaviorName, {} };
	// Add the node to the parent
	parent.children.push_back(newNode);
}

void Tool::UI::UIMonsterProperties::ShowBehaviorConfiguration(BehaviorNode& node) {
	// Display appropriate configuration UI based on node type
	if (node.name == "BehaviorChase") {
		BehaviorChaseConfig();
	}
	else if (node.name == "BehaviorDistanceConditionHelper") {
		BehaviorDistanceConditionHelperConfig();
	}
	else if (node.name == "BehaviorMovementBounce") {
		BehaviorMovementBounceConfig();
	}
	else if (node.name == "BehaviorShootBarrage") {
		BehaviorShootBarrageConfig();
	}
	else if (node.name == "BehaviorShootProjectile") {
		BehaviorShootProjectileConfig();
	}
	else if (node.name == "BehaviorShootStrategyBase") {
		BehaviorShootStrategyBaseConfig();
	}
	else if (node.name == "BehaviorSpreadShot") {
		BehaviorSpreadShotConfig();
	}
	else if (node.name == "BehaviorMultiConfig") {
		// For nested BehaviorMultiConfig, show add behavior option
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
	else {
		ImGui::Text("No configuration available for this behavior type.");
	}
}