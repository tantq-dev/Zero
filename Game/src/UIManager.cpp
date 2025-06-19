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
	Core::EventSystem::getInstance().subscribe(EventKeys::SendMonsterData,
		[this](const Core::EventData& data) {
			std::vector<MonsterTypeDefinition> monsterData = data.get<std::vector<MonsterTypeDefinition>>();
			M_UIMonsterPalette->ImportMonsterData(monsterData);
		});
	Core::EventSystem::getInstance().subscribe(EventKeys::SendMonsterData,
		[this](const Core::EventData& data) {
			std::vector<MonsterTypeDefinition> monsterData = data.get<std::vector<MonsterTypeDefinition>>();
			M_UIMonsterPalette->ImportMonsterData(monsterData);
		});
	Core::EventSystem::getInstance().subscribe(EventKeys::OnMonsterHover,
		[this](const Core::EventData& data) {
			std::string monsterName = data.get<std::string>();
			m_currentMonsterName = monsterName;
			if (monsterName == std::string())
			{
				m_isShowToolTip = false;
			}
			else {
				m_isShowToolTip = true;
			}
		});
}

void Tool::UI::UIManager::Render() {


	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Monster Palette")) {
				m_isShowMonsterPalette = true;
			}

			if (ImGui::MenuItem("Wave Information")) {
				m_isShowWaveInformation = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Export Bullet")) {
				M_UIMonsterPalette->ExportBullet();
			}

			if (ImGui::MenuItem("Import"))
			{
				Core::EventData eventData;
				Core::EventSystem::getInstance().publish(EventKeys::ImportJson, eventData);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}


	if (m_isShowToolTip) {
		ImGui::SetTooltip(m_currentMonsterName.c_str());
	}



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
