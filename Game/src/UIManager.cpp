#include "UIManager.h"
#include "core/EventSystem.h"
#include "EventKey.h"
#include "MonsterModel.h"
void Tool::UI::UIManager::Initialize()
{
	m_UIMonsterPalate = std::make_unique<UIMonsterPalate>();


	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromMap,
		[this](const Core::EventData& data) {
			m_isShowMonsterProperty = true;
		});
}

void Tool::UI::UIManager::Render() {
	if (m_isShowMonsterPalate)
	{
		m_UIMonsterPalate->ShowMonsterPalate(&m_isShowMonsterPalate);
	}
	if (m_isShowMonsterProperty)
	{
		m_UIMonsterProperties->ShowUIMonsterProperties(&m_isShowMonsterProperty);
	}

}
