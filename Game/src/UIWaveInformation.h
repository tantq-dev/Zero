#pragma once
#include "WaveModel.h"
#include "entt.hpp"


namespace Tool::UI {
	class UIWaveInformation {
	public:
		UIWaveInformation();
		void UpdateCurrentWave(int index);
		~UIWaveInformation() = default;

		void DisplayWaveInformation(bool* p_open);

		void GetWaveInfomation(const std::vector<MonsterWave*> waveInfo) {
			m_pWaveInformations = waveInfo;
		}

	private:
		std::vector<MonsterWave*> m_pWaveInformations;
		MonsterWave* m_currentWave = nullptr;
	};
}
