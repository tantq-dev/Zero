#pragma once
#include "ImGui/imgui.h"
#include <string>
#include <vector>
#include <memory>
#include "MonsterModel.h"
#include "BulletRegistry.h"

namespace Tool {
    namespace UI {

        class UIBulletProperties {
        public:
            UIBulletProperties();
            ~UIBulletProperties();
            
            void ShowUIBulletProperties(bool* p_open);
            void SetCurrentBullet(BulletDefinition* bullet);
            
        private:
            BulletDefinition* m_pCurrentBullet = nullptr;
            bool m_hasValidSelection = false;
            
            // UI state
            bool m_dataChanged = false;
            
            void SaveCurrentBullet();
            bool RenderBulletConfigFields(BulletConfig* bulletConfig);
            std::vector<const char*> GetAvailableBullets() const;
        };
    }
}