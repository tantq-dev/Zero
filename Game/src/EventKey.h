#pragma once
#include <string_view>

// Forward declarations
struct BulletDefinition;

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

		// Bullet events from palette
		static constexpr std::string_view BulletSelectedFromPalette = "bullet_selected_from_palette";
		static constexpr std::string_view BulletCreated = "bullet_created";
		static constexpr std::string_view BulletDeleted = "bullet_deleted";
		static constexpr std::string_view BulletUpdated = "bullet_updated";
		static constexpr std::string_view BulletPropertiesChanged = "bullet_properties_changed";

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
		static constexpr std::string_view SendBulletData = "send_bullet_data";

		static constexpr std::string_view ImportMonsterWaveData = "import_monster_wave_data";
		static constexpr std::string_view ImportWaveStructure = "import_wave_structure";

		static constexpr std::string_view OnMonsterHover = "on_monster_hover";
		static constexpr std::string_view OnMonsterLeave = "on_monster_leave";


		// Disallow instantiation
		EventKeys() = delete;
	};
}

// Event structures for bullet-related events
struct EventBulletSelected {
	BulletDefinition* bullet;
};

struct EventBulletPropertiesChanged {
	BulletDefinition* bullet;
};