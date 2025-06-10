#pragma once
#include "ImGui/imgui.h"
#include "UIMonsterPalate.h"
#include <iostream>
namespace Tool {
	namespace UI {

		class UIManager {
		public:
			void Initialize();
			void Render();
		private:

			std::unique_ptr<UIMonsterPalate> m_UIMonsterPalate;

			bool m_isShowMonsterPalate = true;

		};

	}

}