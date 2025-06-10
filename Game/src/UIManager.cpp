#include "UIManager.h"

void Tool::UI::UIManager::Initialize()
{
	m_UIMonsterPalate = std::make_unique<UIMonsterPalate>();
}

void Tool::UI::UIManager::Render() {
	if (m_isShowMonsterPalate)
	{
		m_UIMonsterPalate->ShowMonsterPalate(&m_isShowMonsterPalate);
	}

}
