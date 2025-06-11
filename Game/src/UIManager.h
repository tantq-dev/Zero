#pragma once
#include "UIMonsterPalate.h"
#include <iostream>
#include "UIMonsterProperties.h"
namespace Tool {
	namespace UI {

		class UIManager {
		public:
			void Initialize();
			void Render();
			std::unique_ptr<UIMonsterProperties> M_UIMonsterProperties;
			std::unique_ptr<UIMonsterPalate> M_UIMonsterPalate;
		private:
			bool m_isShowMonsterPalate = true;
			bool m_isShowMonsterProperty = false;

		};

	}

}