#include "UIManager.h"
#include "core/EventSystem.h"
#include "EventKey.h"
#include "MonsterModel.h"
#include "BulletRegistry.h"
#include "utilities/Logger.h"
#include "DataHandler.h"

Tool::UI::UIManager::UIManager()
{
	Initialize();
}

void Tool::UI::UIManager::Initialize()
{
	M_UIMonsterPalette = std::make_unique<UIMonsterPalette>();
	M_UIMonsterProperties = std::make_unique<UIMonsterProperties>();
	M_UIBulletPalette = std::make_unique<UIBulletPalette>();
	M_UIBulletProperties = std::make_unique<UIBulletProperties>();
	M_UIWaveInformation = std::make_unique<UIWaveInformation>();

	// Monster event subscriptions
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

	// Bullet event subscriptions
	Core::EventSystem::getInstance().subscribe(EventKeys::BulletSelectedFromPalette,
		[this](const Core::EventData& data) {
			BulletDefinition* bullet = data.get<BulletDefinition*>();
			M_UIBulletProperties->SetCurrentBullet(bullet);
			m_isShowBulletProperty = true;
		});

	Core::EventSystem::getInstance().subscribe(EventKeys::SendBulletData,
		[this](const Core::EventData& data) {
			// Import bullets directly from DataHandler instead of using event data
			ImportBulletsFromDataHandler();
		});
}

void Tool::UI::UIManager::SetBulletConfig(std::vector<BulletDefinition> data) {
	M_UIBulletPalette->ImportBulletData(data);
}

void Tool::UI::UIManager::ImportBulletsFromDataHandler() {
	if (!m_dataHandler) {
		LOG_ERROR("UIManager: DataHandler not set, cannot import bullets");
		return;
	}

	try {
		// Get imported bullet configs directly from DataHandler
		const auto& bulletConfigs = m_dataHandler->GetImportedBulletConfigs();
		
		if (bulletConfigs.empty()) {
			LOG_INFO("UIManager: No bullets to import from DataHandler");
			return;
		}

		// Convert to BulletDefinition vector for the palette
		std::vector<BulletDefinition> bulletDefinitions;
		for (const auto& [bulletId, bulletConfig] : bulletConfigs) {
			BulletDefinition bulletDef;
			bulletDef.config = bulletConfig;
			bulletDef.name = bulletConfig.name.empty() ? bulletId : bulletConfig.name;
			bulletDef.textureName = bulletConfig.validBulletIngame;
			bulletDef.id = std::stoi(bulletId);
			bulletDefinitions.push_back(bulletDef);
		}
		
		// Import into bullet palette (this will update the UI)
		M_UIBulletPalette->ImportBulletData(bulletDefinitions);
		LOG_INFO("UIManager: Successfully updated bullet palette with " + std::to_string(bulletDefinitions.size()) + " imported bullets");
		
	} catch (const std::exception& e) {
		LOG_ERROR("UIManager: Error importing bullets from DataHandler: " + std::string(e.what()));
	}
}

void Tool::UI::UIManager::Render() {


	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Monster Palette")) {
				m_isShowMonsterPalette = true;
			}

			if (ImGui::MenuItem("Bullet Palette")) {
				m_isShowBulletPalette = true;
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
		M_UIMonsterProperties->ShowUIMonsterProperties(&m_isShowMonsterProperty, M_UIBulletPalette->GetBulletRegistry().GetBulletTypeIds());
	}
	if (m_isShowBulletPalette)
	{
		M_UIBulletPalette->ShowBulletPalette(&m_isShowBulletPalette);
	}
	if (m_isShowBulletProperty)
	{
		M_UIBulletProperties->ShowUIBulletProperties(&m_isShowBulletProperty);
	}
	if (m_isShowWaveInformation)
	{
		M_UIWaveInformation->DisplayWaveInformation(&m_isShowWaveInformation);
	}

}
