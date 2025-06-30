#include "UIBulletProperties.h"
#include <imgui.h>
#include <vector>
#include <string>
#include "EventKey.h"
#include "core/EventSystem.h"
#include "utilities/Logger.h"

Tool::UI::UIBulletProperties::UIBulletProperties()
{
}

Tool::UI::UIBulletProperties::~UIBulletProperties()
{
}

void Tool::UI::UIBulletProperties::ShowUIBulletProperties(bool* p_open, std::vector<std::string> currentBulletIDs)
{
	if (p_open) {
		bool window_contents_visible = ImGui::Begin("Bullet Properties", p_open, ImGuiWindowFlags_MenuBar);
		if (!window_contents_visible) {
			ImGui::End();
			return;
		}

		bool dataChanged = false;  // Track if any data changed

		if (m_hasValidSelection && m_pCurrentBullet) {
			// Menu bar
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Save")) {
						SaveCurrentBullet();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// Basic bullet information
			ImGui::Text("Bullet ID: %d", m_pCurrentBullet->id);
			ImGui::Separator();

			// Bullet name (read-only display)
			ImGui::Text("Name: %s", m_pCurrentBullet->name.c_str());

			// Texture name (read-only display)
			ImGui::Text("Texture: %s", m_pCurrentBullet->textureName.c_str());

			ImGui::Separator();

			// Bullet configuration
			ImGui::Text("Bullet Configuration:");
			if (ImGui::TreeNode("Config")) {
				if (RenderBulletConfigFields(&m_pCurrentBullet->config, currentBulletIDs)) {
					dataChanged = true;
				}
				ImGui::TreePop();
			}

			// Auto-save if data changed
			if (dataChanged) {
				SaveCurrentBullet();
			}
		}
		else {
			ImGui::Text("No bullet selected");
			ImGui::Text("Select a bullet from the Bullet Palette to edit its properties.");
		}

		ImGui::End();
	}
}

void Tool::UI::UIBulletProperties::SetCurrentBullet(BulletDefinition* bullet)
{
	m_pCurrentBullet = bullet;
	m_hasValidSelection = (bullet != nullptr);
}

void Tool::UI::UIBulletProperties::SaveCurrentBullet()
{
	if (m_pCurrentBullet && m_hasValidSelection) {        // Send event that bullet properties have been updated
		Core::EventData eventData;
		eventData.data = m_pCurrentBullet;
		Core::EventSystem::getInstance().publish(EventKeys::BulletPropertiesChanged, eventData);

		LOG_INFO("Bullet properties saved for: " + m_pCurrentBullet->name);
	}
}

bool Tool::UI::UIBulletProperties::RenderBulletSpawnerConfigField(SpawnerBulletConfig* bulletSpawnerConfig, std::vector<std::string> currentBulletIDs)
{
	bool changed = false;

	// Get available bullets from the bullet registry
	std::vector<std::string> availableBullets = currentBulletIDs;
	std::vector<const char*> bulletOptions;
	for (const auto& bullet : availableBullets) {
		bulletOptions.push_back(bullet.c_str());
	}

	// Find current bullet index
	int currentBulletIndex = 0;
	for (int i = 0; i < availableBullets.size(); i++) {
		if (bulletSpawnerConfig->bulletId == availableBullets[i]) {
			currentBulletIndex = i;
			break;
		}
	}

	// Bullet type dropdown
	if (ImGui::Combo("Valid Bullet ", &currentBulletIndex, bulletOptions.data(), static_cast<int>(bulletOptions.size()))) {
		if (currentBulletIndex < availableBullets.size()) {
			bulletSpawnerConfig->bulletId = availableBullets[currentBulletIndex];
			changed = true;
		}
	}


	// Bullet type
	std::vector<std::string> bulletTypeString = GetSpawnerTypeString();
	const char* bulletTypeNames[static_cast<int>(SpawnerType::COUNT)] = {};

	for (size_t i = 0; i < bulletTypeString.size(); i++)
	{
		bulletTypeNames[i] = bulletTypeString[i].c_str();
	}

	int bulletType = static_cast<int>(bulletSpawnerConfig->spawnerType);
	if (ImGui::Combo("Spawner Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
		bulletSpawnerConfig->spawnerType = static_cast<SpawnerType>(bulletType);
		changed = true;
	}

	if (ImGui::InputFloat("Time Activate", &bulletSpawnerConfig->timeActivate)) {
		changed = true;
	}

	if (ImGui::InputInt("Num of Bullet", &bulletSpawnerConfig->numOfBullet)) {
		changed = true;
	}

	if (ImGui::InputInt("Spread Angle", &bulletSpawnerConfig->spreadAngle)) {
		changed = true;
	}

	return changed;
}

bool Tool::UI::UIBulletProperties::RenderBulletConfigFields(BulletConfig* bulletConfig, std::vector<std::string> currentBulletIDs)
{
	bool changed = false;

	// Get available bullets
	std::vector<const char*> validBulletsIngame = GetAvailableBulletConfig();

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
	if (ImGui::InputInt("Speed", &bulletConfig->speed)) {
		changed = true;
	}

	if (ImGui::InputInt("Alive Time", &bulletConfig->aliveTime)) {
		changed = true;
	}

	if (ImGui::InputInt("Damage", &bulletConfig->damage)) {
		changed = true;
	}

	if (ImGui::InputInt("Bounce", &bulletConfig->bounce)) {
		changed = true;
	}

	// Add new behavior to root
	if (ImGui::Button("Add Spawner Bullet Bahavior") && !bulletConfig->spawnerBullet) {

		bulletConfig->spawnerBullet = std::make_unique<SpawnerBulletConfig>();
	}
	ImGui::SameLine();
	if (bulletConfig->spawnerBullet && ImGui::Button("Remove")) {
		bulletConfig->spawnerBullet = nullptr;
	}

	if (bulletConfig->spawnerBullet) {
		RenderBulletSpawnerConfigField(&*bulletConfig->spawnerBullet, currentBulletIDs);
	}

	return changed;
}

//std::vector<const int> Tool::UI::UIBulletProperties::GetAvailableBulletIDs() const
//{
//	
//
//	return a;
//}

std::vector<const char*> Tool::UI::UIBulletProperties::GetAvailableBulletConfig() const
{
	std::vector<const char*> validBulletsIngame;

	for (auto& [name, texture] : BulletTextureMap) {
		validBulletsIngame.push_back(name.c_str());
	}

	return validBulletsIngame;
}
