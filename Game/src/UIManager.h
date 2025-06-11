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
		private:

			std::unique_ptr<UIMonsterPalate> m_UIMonsterPalate;
			std::unique_ptr<UIMonsterProperties> m_UIMonsterProperties;


			bool m_isShowMonsterPalate = true;
			bool m_isShowMonsterProperty = false;

		};

	}

}