#include "UIManager.h"
#include "core/EventSystem.h"
#include "EventKey.h"
#include "MonsterModel.h"

Tool::UI::UIManager::UIManager()
{
	Initialize();
}

void Tool::UI::UIManager::Initialize()
{
	M_UIMonsterPalette = std::make_unique<UIMonsterPalette>();
	M_UIMonsterProperties = std::make_unique<UIMonsterProperties>();
	M_UIWaveInformation = std::make_unique<UIWaveInformation>();

	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromPalette,
		[this](const Core::EventData& data) {
			MonsterTypeDefinition pros = data.get<MonsterTypeDefinition>();
			M_UIMonsterProperties->SetCurrentProperties(pros);
			m_isShowMonsterProperty = true;
		});
}

void Tool::UI::UIManager::Render() {
	if (m_isShowMonsterPalette)
	{
		M_UIMonsterPalette->ShowMonsterPalette(&m_isShowMonsterPalette);
	}
	if (m_isShowMonsterProperty)
	{
		M_UIMonsterProperties->ShowUIMonsterProperties(&m_isShowMonsterProperty);
	}
	if (m_isShowWaveInformation)
	{
		M_UIWaveInformation->DisplayWaveInformation(&m_isShowWaveInformation);
	}

}
