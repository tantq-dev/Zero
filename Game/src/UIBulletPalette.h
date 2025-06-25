#pragma once
#include "ImGui/imgui.h"
#include <string>
#include <vector>
#include "MonsterModel.h"
#include "BulletRegistry.h"

namespace Tool {
    namespace UI {

        class UIBulletPalette {
        public:
            UIBulletPalette();
            ~UIBulletPalette() = default;
            void ShowBulletPalette(bool* p_open);
            void InvalidateCache() { m_cacheNeedsUpdate = true; }
            void ImportBulletData(const std::vector<BulletDefinition>& data);
            BulletDefinition* GetSelectedBullet();
            BulletRegistry& GetBulletRegistry() { return m_bulletRegistry; }
            
        private:
            BulletRegistry m_bulletRegistry;

            std::vector<BulletDefinition*> m_cachedBulletTypes;
            bool m_cacheNeedsUpdate = true;

            float           IconSize = 64.0f;
            int             IconSpacing = 10;
            int             IconHitSpacing = 4;
            bool            StretchSpacing = true;
            int             NextItemId = 0;
            int             selectedItem = -1;

            ImVec2          LayoutItemSize;
            ImVec2          LayoutItemStep;
            float           LayoutItemSpacing = 0.0f;
            float           LayoutSelectableSpacing = 0.0f;
            float           LayoutOuterPadding = 0.0f;
            int             LayoutColumnCount = 0;
            int             LayoutLineCount = 0;

            bool m_showAddBulletPopup = false;
            char m_newBulletName[256] = "";

            // Members for texture selection
            bool m_showTextureDropdown = false;
            std::string m_selectedTextureName;
            std::vector<std::string> m_allValidBullets;
            std::string m_selectedValidBullet;
            int m_selectedTextureIndex = 0;

            // Members for bullet type selection
            BulletType m_selectedBulletType = BulletType::Straight;
            int m_bulletSpeed = 100;
            int m_bulletDamage = 10;
            int m_bulletAliveTime = 60;
            int m_bulletBounce = 0;

            void ShowAddBulletPopup();
            void AddItem(const std::string& validBullet, const std::string& name, const std::string& textureName, const BulletConfig& config);
            void UpdateLayoutSizes(float avail_width);
            void UpdateCache();
            void UpdateAvailableTextures();
            void ShowTextureSelectionDropdown();
            void ResetBulletConfigFields();
        };
    }
}
