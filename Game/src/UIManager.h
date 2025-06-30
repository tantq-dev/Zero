#pragma once
#include "UIMonsterPalette.h"
#include "UIBulletPalette.h"
#include "UIBulletProperties.h"
#include <iostream>
#include "UIMonsterProperties.h"
#include "UIWaveInformation.h"
namespace Tool {
	namespace UI {

		class UIManager {
		public:
			UIManager();
			void Initialize();
			void SetBulletConfig(std::vector<BulletDefinition> data);
			void Render();
			BulletRegistry* GetBulletRegistry() { return &(M_UIBulletPalette->GetBulletRegistry()); }
			std::unique_ptr<UIMonsterProperties> M_UIMonsterProperties;
			std::unique_ptr<UIMonsterPalette> M_UIMonsterPalette;
			std::unique_ptr<UIBulletPalette> M_UIBulletPalette;
			std::unique_ptr<UIBulletProperties> M_UIBulletProperties;
			std::unique_ptr<UIWaveInformation> M_UIWaveInformation;
		private:
			std::string m_currentMonsterName = std::string();
			bool m_isShowMonsterPalette = true;
			bool m_isShowMonsterProperty = false;
			bool m_isShowBulletPalette = false;
			bool m_isShowBulletProperty = false;
			bool m_isShowWaveInformation = true;
			bool m_isShowToolTip = false;
		};

	}

}