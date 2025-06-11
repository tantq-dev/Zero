#include "UIManager.h"
#include "core/EventSystem.h"
#include "EventKey.h"
#include "MonsterModel.h"
void Tool::UI::UIManager::Initialize()
{
	M_UIMonsterPalate = std::make_unique<UIMonsterPalate>();
	M_UIMonsterProperties = std::make_unique<UIMonsterProperties>();

	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromMap,
		[this](const Core::EventData& data) {
			M_UIMonsterProperties->SetCurrentProperties(data.get<PlacedMonster*>()->properties);
			m_isShowMonsterProperty = true;
		});
}

void Tool::UI::UIManager::Render() {
	if (m_isShowMonsterPalate)
	{
		M_UIMonsterPalate->ShowMonsterPalate(&m_isShowMonsterPalate);
	}
	if (m_isShowMonsterProperty)
	{
		M_UIMonsterProperties->ShowUIMonsterProperties(&m_isShowMonsterProperty);
	}

}
