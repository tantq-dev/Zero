#pragma once
#include "ImGui/imgui.h"
#include <string>
#include <vector>
#include "MonsterModel.h"



namespace Tool {
	namespace UI {

		class UIMonsterPalate {
		public:
			UIMonsterPalate();
			~UIMonsterPalate() = default;
			void ShowMonsterPalate(bool* p_open);
		private:
			std::vector<MonsterItem> MonsterItems;

			float           IconSize = 64.0f;
			int             IconSpacing = 10;
			int             IconHitSpacing = 4;
			bool            StretchSpacing = true;
			int				NextItemId = 0;
			int				selectedItem = 0;

			ImVec2          LayoutItemSize;
			ImVec2          LayoutItemStep;
			float           LayoutItemSpacing = 0.0f;
			float           LayoutSelectableSpacing = 0.0f;
			float           LayoutOuterPadding = 0.0f;
			int             LayoutColumnCount = 0;
			int             LayoutLineCount = 0;

			void AddItems(int count);

			void AddItems(std::vector<std::string> monsterNameList);

			void UpdateLayoutSizes(float avail_width);
		};
	}
}
