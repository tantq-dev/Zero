#pragma once
#include "WaveModel.h"

namespace Tool::UI {
	class UIWaveInformation {
	public:
		UIWaveInformation();
		~UIWaveInformation() = default;

		void DisplayWaveInformation(bool* p_open);

		void GetWaveInfomation(WaveInformation waveInfo) {
			m_pWaveInformation = waveInfo;
		}

	private:
		WaveInformation m_pWaveInformation = { 0,1,1 };
	};
}
