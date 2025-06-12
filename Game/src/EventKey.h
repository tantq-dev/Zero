#pragma once
#include <string_view>

namespace Tool {
	class EventKeys {
	public:
		// Monster events from palate
		static constexpr std::string_view MonsterSelectedFromPalette = "monster_selected_from_palate";
		static constexpr std::string_view MonsterCreated = "monster_created";
		static constexpr std::string_view MonsterDeleted = "monster_deleted";
		// Monster events from map
		static constexpr std::string_view MonsterSelectedFromMap = "monster_selected_from_map";

		//Wave event
		static constexpr std::string_view SendWaves = "send_waves";
		static constexpr std::string_view SwitchWave = "switch_wave";



		// Disallow instantiation
		EventKeys() = delete;
	};
}