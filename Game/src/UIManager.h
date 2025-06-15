#pragma once
#include "UIMonsterPalette.h"
#include <iostream>
#include "UIMonsterProperties.h"
#include "UIWaveInformation.h"
namespace Tool {
	namespace UI {

		class UIManager {
		public:
			UIManager();
			void Initialize();
			void Render();
			std::unique_ptr<UIMonsterProperties> M_UIMonsterProperties;
			std::unique_ptr<UIMonsterPalette> M_UIMonsterPalette;
			std::unique_ptr<UIWaveInformation> M_UIWaveInformation;
		private:
			bool m_isShowMonsterPalette = true;
			bool m_isShowMonsterProperty = false;
			bool m_isShowWaveInformation = true;
		};

	}

}