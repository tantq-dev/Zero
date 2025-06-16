#pragma once
#include "ImGui/imgui.h"
#include <string>
#include <vector>
#include "MonsterModel.h"
#include "MonsterTypeRegistry.h"


namespace Tool {
	namespace UI {

		class UIMonsterPalette {
		public:
			UIMonsterPalette();
			~UIMonsterPalette() = default;
			void ShowMonsterPalette(bool* p_open);
			void InvalidateCache() { m_cacheNeedsUpdate = true; }
		private:
			MonsterTypeRegistry m_monsterTypeRegistry;

			std::vector<MonsterTypeDefinition*> m_cachedMonsterTypes;
			bool m_cacheNeedsUpdate = true;

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

			bool m_showAddMonsterPopup;
			char m_newMonsterName[256];

			//  members for texture selection
			bool m_showTextureDropdown;
			std::string m_selectedTextureName;
			std::vector<std::string> m_allValidMonster;
			std::string m_selectedValidMonster;
			int m_selectedTextureIndex;

			// Add this method declaration:
			void ShowAddMonsterPopup();

			void AddItem(std::string validMonster, std::string name, std::string textureName);

			void UpdateLayoutSizes(float avail_width);
			void UpdateCache();
			void UpdateAvailableTextures();
			void ShowTextureSelectionDropdown();
		};
	}
}
