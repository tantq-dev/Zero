#include "UIBulletPalette.h"
#include <imgui.h>
#include "utilities/Logger.h"
#include "EventKey.h"
#include "core/EventSystem.h"

Tool::UI::UIBulletPalette::UIBulletPalette()
{
    UpdateAvailableTextures();
}

void Tool::UI::UIBulletPalette::ShowBulletPalette(bool* p_open)
{
    if (!p_open || !*p_open) return;

    bool window_contents_visible = ImGui::Begin("Bullet Palette", p_open, ImGuiWindowFlags_MenuBar);
    if (!window_contents_visible) {
        ImGui::End();
        return;
    }

    // Menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Bullet")) {
            if (ImGui::MenuItem("Add New Bullet")) {
                m_showAddBulletPopup = true;
                ResetBulletConfigFields();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Update cache if needed
    if (m_cacheNeedsUpdate) {
        UpdateCache();
    }

    // Layout calculation
    float avail_width = ImGui::GetContentRegionAvail().x;
    UpdateLayoutSizes(avail_width);

    // Display bullets
    for (int n = 0; n < m_cachedBulletTypes.size(); n++) {
        BulletDefinition* bullet = m_cachedBulletTypes[n];
        ImGui::PushID(n);

        // Calculate position
        int column = n % LayoutColumnCount;
        int line = n / LayoutColumnCount;
        
        ImVec2 pos = ImVec2(
            LayoutOuterPadding + (float)column * LayoutItemStep.x,
            LayoutOuterPadding + (float)line * LayoutItemStep.y
        );
        ImGui::SetCursorPos(pos);

        // Selectable item
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 1.0f));
        bool selected = (selectedItem == n);
        
        std::string label = bullet->name;
        if (ImGui::Selectable(label.c_str(), selected, 0, LayoutItemSize)) {
            selectedItem = n;
              // Send event that bullet was selected
            Core::EventData eventData;
            eventData.data = bullet;
            Core::EventSystem::getInstance().publish(EventKeys::BulletSelectedFromPalette, eventData);
        }
        ImGui::PopStyleVar();

        // Right-click context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                // Handle bullet deletion
                LOG_INFO("Delete bullet: " + bullet->name);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    // Show add bullet popup
    if (m_showAddBulletPopup) {
        ShowAddBulletPopup();
    }

    ImGui::End();
}

void Tool::UI::UIBulletPalette::ShowAddBulletPopup()
{
    ImGui::OpenPopup("Add New Bullet");
    
    if (ImGui::BeginPopupModal("Add New Bullet", &m_showAddBulletPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        // Bullet name input
        ImGui::InputText("Bullet Name", m_newBulletName, sizeof(m_newBulletName));
        
        // Bullet type selection
        std::vector<std::string> bulletTypeStrings = GetBulletsTypeString();
        const char* bulletTypeNames[static_cast<int>(BulletType::COUNT)] = {};
        
        for (size_t i = 0; i < bulletTypeStrings.size(); i++) {
            bulletTypeNames[i] = bulletTypeStrings[i].c_str();
        }
        
        int bulletType = static_cast<int>(m_selectedBulletType);
        if (ImGui::Combo("Bullet Type", &bulletType, bulletTypeNames, IM_ARRAYSIZE(bulletTypeNames))) {
            m_selectedBulletType = static_cast<BulletType>(bulletType);
        }
        
        // Bullet properties
        ImGui::InputInt("Speed", &m_bulletSpeed);
        ImGui::InputInt("Damage", &m_bulletDamage);
        ImGui::InputInt("Alive Time", &m_bulletAliveTime);
        ImGui::InputInt("Bounce", &m_bulletBounce);
        
        // Valid bullet selection
        if (ImGui::Button("Select Texture")) {
            m_showTextureDropdown = true;
        }
        ImGui::SameLine();
        ImGui::Text("Selected: %s", m_selectedValidBullet.c_str());
        
        ShowTextureSelectionDropdown();
        
        ImGui::Separator();
        
        // Buttons
        if (ImGui::Button("Create")) {
            if (strlen(m_newBulletName) > 0) {
                // Create bullet config
                BulletConfig config;
                config.bulletType = m_selectedBulletType;
                config.speed = m_bulletSpeed;
                config.damage = m_bulletDamage;
                config.aliveTime = m_bulletAliveTime;
                config.bounce = m_bulletBounce;
                config.validBulletIngame = m_selectedValidBullet;
                
                // Add to registry
                AddItem(m_selectedValidBullet, m_newBulletName, m_selectedTextureName, config);
                
                // Reset and close
                ResetBulletConfigFields();
                m_showAddBulletPopup = false;
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            m_showAddBulletPopup = false;
        }
        
        ImGui::EndPopup();
    }
}

void Tool::UI::UIBulletPalette::AddItem(const std::string& validBullet, const std::string& name, const std::string& textureName, const BulletConfig& config)
{
    m_bulletRegistry.RegisterBulletType(name, config, textureName);
    InvalidateCache();
}

void Tool::UI::UIBulletPalette::UpdateLayoutSizes(float avail_width)
{
    // Calculate layout similar to monster palette
    LayoutItemSpacing = (float)IconSpacing;
    LayoutSelectableSpacing = (float)IconHitSpacing;
    LayoutOuterPadding = ImGui::GetStyle().WindowPadding.x * 0.5f;
    
    float effective_width = avail_width - LayoutOuterPadding * 2.0f;
    LayoutColumnCount = fmax(1, (int)((effective_width + LayoutItemSpacing) / (IconSize + LayoutItemSpacing)));
    
    if (StretchSpacing && LayoutColumnCount > 1) {
        LayoutItemSpacing = (effective_width - IconSize * LayoutColumnCount) / (LayoutColumnCount - 1);
    }
    
    LayoutItemSize = ImVec2(IconSize, IconSize + ImGui::GetTextLineHeightWithSpacing());
    LayoutItemStep = ImVec2(IconSize + LayoutItemSpacing, LayoutItemSize.y + LayoutSelectableSpacing);
    
    LayoutLineCount = (int)((m_cachedBulletTypes.size() + LayoutColumnCount - 1) / LayoutColumnCount);
}

void Tool::UI::UIBulletPalette::UpdateCache()
{
    m_cachedBulletTypes = m_bulletRegistry.GetAllBulletTypes();
    m_cacheNeedsUpdate = false;
}

void Tool::UI::UIBulletPalette::UpdateAvailableTextures()
{
    m_allValidBullets.clear();
    for (const auto& [name, texture] : BulletTextureMap) {
        m_allValidBullets.push_back(name);
    }
    
    if (!m_allValidBullets.empty()) {
        m_selectedValidBullet = m_allValidBullets[0];
        m_selectedTextureName = BulletTextureMap[m_selectedValidBullet];
    }
}

void Tool::UI::UIBulletPalette::ShowTextureSelectionDropdown()
{
    if (m_showTextureDropdown) {
        ImGui::SetNextWindowPos(ImGui::GetItemRectMin());
        ImGui::SetNextWindowSize(ImVec2(250.0f, 200.0f));
        
        if (ImGui::Begin("##BulletTextureDropdown", &m_showTextureDropdown, 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {
            
            for (int i = 0; i < m_allValidBullets.size(); i++) {
                bool selected = (m_selectedTextureIndex == i);
                if (ImGui::Selectable(m_allValidBullets[i].c_str(), selected)) {
                    m_selectedTextureIndex = i;
                    m_selectedValidBullet = m_allValidBullets[i];
                    m_selectedTextureName = BulletTextureMap[m_selectedValidBullet];
                    m_showTextureDropdown = false;
                }
            }
        }
        ImGui::End();
        
        if (!ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
            m_showTextureDropdown = false;
        }
    }
}

void Tool::UI::UIBulletPalette::ResetBulletConfigFields()
{
    memset(m_newBulletName, 0, sizeof(m_newBulletName));
    m_selectedBulletType = BulletType::Straight;
    m_bulletSpeed = 100;
    m_bulletDamage = 10;
    m_bulletAliveTime = 60;
    m_bulletBounce = 0;
    m_selectedTextureIndex = 0;
    if (!m_allValidBullets.empty()) {
        m_selectedValidBullet = m_allValidBullets[0];
    }
}

void Tool::UI::UIBulletPalette::ImportBulletData(const std::vector<BulletDefinition>& data)
{
    m_bulletRegistry.RegisterBulletTypeFromData(data);
    UpdateCache();
}

BulletDefinition* Tool::UI::UIBulletPalette::GetSelectedBullet()
{
    if (selectedItem >= 0 && selectedItem < m_cachedBulletTypes.size()) {
        return m_cachedBulletTypes[selectedItem];
    }
    return nullptr;
}
