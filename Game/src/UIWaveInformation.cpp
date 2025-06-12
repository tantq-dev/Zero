#include "core/EventSystem.h"
#include "UIWaveInformation.h"
#include "imgui.h"
#include "EventKey.h"

Tool::UI::UIWaveInformation::UIWaveInformation()
{
	Core::EventSystem::getInstance().subscribe(EventKeys::SendWaves,
		[this](const Core::EventData& data) {
			GetWaveInfomation(data.get<WaveInformation>());
		});
}

void Tool::UI::UIWaveInformation::DisplayWaveInformation(bool* p_open)
{
	if (p_open) {
		bool window_contents_visible = ImGui::Begin("Wave Information", p_open, ImGuiWindowFlags_MenuBar);
		if (!window_contents_visible) {
			ImGui::End();
			return;
		}


		if (ImGui::Button("Previous wave"))
		{

			Core::EventData eventData;
			eventData.data = -1;
			Core::EventSystem::getInstance().publish(EventKeys::SwitchWave, eventData);
		}
		ImGui::SameLine();
		ImGui::Text("Wave Index: %s", std::to_string(m_pWaveInformation.waveIndex).c_str());

		ImGui::SameLine();

		std::string nextButtonName = m_pWaveInformation.isLastWave ? "Add wave" : "Next wave";

		if (ImGui::Button(nextButtonName.c_str()))
		{
			Core::EventData eventData;
			eventData.data = 1;
			Core::EventSystem::getInstance().publish(EventKeys::SwitchWave, eventData);
		}

		ImGui::Separator();
		ImGui::Text("Is Boss Wave: %s", std::to_string(m_pWaveInformation.isBossWave).c_str());

		ImGui::End();
	}
}
