#pragma once
#include <string_view>

namespace Tool {
	class EventKeys {
	public:
		// Monster events from palate
		static constexpr std::string_view MonsterSelectedFromPalette = "monster_selected_from_palate";
		static constexpr std::string_view MonsterCreated = "monster_created";
		static constexpr std::string_view MonsterDeleted = "monster_deleted";
		static constexpr std::string_view MonsterUpdated = "monster_updated";
		// Monster events from map
		static constexpr std::string_view MonsterSelectedFromMap = "monster_selected_from_map";

		//Wave event
		static constexpr std::string_view SendWaves = "send_waves";
		static constexpr std::string_view SwitchWave = "switch_wave";
		static constexpr std::string_view UISwitchWave = "ui_switch_wave";
		static constexpr std::string_view AddWave = "add_wave";

		//Data handler event
		static constexpr std::string_view ExportBullet = "export_bullet";
		static constexpr std::string_view ExportWaves = "export_waves";

		static constexpr std::string_view ImportJson = "import";
		static constexpr std::string_view SendMonsterData = "send_monster_data";
		static constexpr std::string_view ImportMonsterWaveData = "import_monster_wave_data";
		static constexpr std::string_view ImportWaveStructure = "import_wave_structure";




		// Disallow instantiation
		EventKeys() = delete;
	};
}