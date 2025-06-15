#include "core/EventSystem.h"
#include "UIWaveInformation.h"
#include "imgui.h"
#include "EventKey.h"
#include "utilities/Logger.h"

Tool::UI::UIWaveInformation::UIWaveInformation()
{
	Core::EventSystem::getInstance().subscribe(EventKeys::SendWaves,
		[this](const Core::EventData& data) {
			GetWaveInfomation(data.get<std::vector<MonsterWave*>>());
		});
	Core::EventSystem::getInstance().subscribe(EventKeys::UISwitchWave,
		[this](const Core::EventData& data) {
			UpdateCurrentWave(data.get<int>());
		});
}

void Tool::UI::UIWaveInformation::UpdateCurrentWave(int index) {
	m_currentWave = m_pWaveInformations[index];
}

void Tool::UI::UIWaveInformation::DisplayWaveInformation(bool* p_open)
{
	if (p_open) {
		bool window_contents_visible = ImGui::Begin("Wave Information", p_open, ImGuiWindowFlags_MenuBar);
		if (!window_contents_visible) {
			ImGui::End();
			return;
		}

		// Add Wave button at the top
		if (ImGui::Button("Add Wave")) {
			Core::EventData eventData;
			eventData.data = 1;
			Core::EventSystem::getInstance().publish(EventKeys::AddWave, eventData);
		}
		ImGui::Separator();

		// Current wave display section (without navigation buttons)
		if (m_currentWave != nullptr) {
			ImGui::Text("Current Wave: %d", m_currentWave->waveIndex);
			ImGui::Text("Is Boss Wave: %s", m_currentWave->isBossWave ? "Yes" : "No");
			ImGui::Text("Monster Count: %zu", m_currentWave->monsterEntities.size());
			ImGui::Separator();
		}

		// Tree node section for all waves with switching functionality
		ImGui::Text("Wave Selection:");

		if (!m_pWaveInformations.empty()) {
			for (size_t i = 0; i < m_pWaveInformations.size(); ++i) {
				MonsterWave* wave = m_pWaveInformations[i];
				if (wave == nullptr) continue;

				// Create a unique ID for each tree node with monster count
				std::string nodeLabel = "Wave " + std::to_string(wave->waveIndex) +
					" (" + std::to_string(wave->monsterEntities.size()) + " monsters)";
				if (wave->isBossWave) {
					nodeLabel += " [BOSS]";
				}

				// Highlight current wave
				bool isCurrentWave = (m_currentWave != nullptr && wave->waveIndex == m_currentWave->waveIndex);
				if (isCurrentWave) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow color
				}

				// Make tree node selectable and switch wave when clicked
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				if (isCurrentWave) {
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), flags);

				// Check if tree node was clicked (but not on the arrow)
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
					// Switch to this wave
					Core::EventData eventData;
					eventData.data = static_cast<int>(i); // Use array index
					Core::EventSystem::getInstance().publish(EventKeys::SwitchWave, eventData);
				}

				if (nodeOpen) {
					// Display wave details inside the tree node
					ImGui::Text("Wave Index: %d", wave->waveIndex);
					ImGui::Text("Is Boss Wave: %s", wave->isBossWave ? "Yes" : "No");
					ImGui::Text("Total Monsters: %zu", wave->monsterEntities.size());

					// Display individual monster entities
					if (!wave->monsterEntities.empty()) {
						ImGui::Separator();
						ImGui::Text("Monster Entities:");
						ImGui::Indent();

						for (size_t j = 0; j < wave->monsterEntities.size(); ++j) {
							entt::entity entity = wave->monsterEntities[j];
							ImGui::Text("Monster %zu: Entity ID %u", j + 1, static_cast<unsigned int>(entity));
						}

						ImGui::Unindent();
					}
					else {
						ImGui::Text("No monsters in this wave");
					}

					ImGui::TreePop();
				}

				if (isCurrentWave) {
					ImGui::PopStyleColor();
				}
			}
		}
		else {
			ImGui::Text("No waves available");
		}

		ImGui::End();
	}
}