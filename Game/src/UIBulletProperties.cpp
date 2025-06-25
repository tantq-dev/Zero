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

void Tool::UI::UIBulletProperties::ShowUIBulletProperties(bool* p_open)
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
                if (RenderBulletConfigFields(&m_pCurrentBullet->config)) {
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

bool Tool::UI::UIBulletProperties::RenderBulletConfigFields(BulletConfig* bulletConfig)
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

    return changed;
}

std::vector<const char*> Tool::UI::UIBulletProperties::GetAvailableBullets() const
{
    std::vector<const char*> validBulletsIngame;

    for (auto& [name, texture] : BulletTextureMap) {
        validBulletsIngame.push_back(name.c_str());
    }

    return validBulletsIngame;
}
