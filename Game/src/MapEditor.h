#pragma once
#include "MonsterModel.h"
#include "utilities/Vec2.h"
#include "entt.hpp"
#include "WaveModel.h"

namespace Tool
{
	class MapEditor
	{
	public:
		MapEditor();
		void AddWave();
		~MapEditor();
		void AddMonsterToMap(Vec2 clickPosition, Vec2 gridPosition);
		void DeleteFromMap(Vec2 clickPosition);
		void OnMonsterSelectedFromUI(MonsterTypeDefinition monster);
		void ClickOnMap(Vec2 clickPosition, Vec2 gridPosition);
		void ClickOnPlacedMonster(entt::entity& e, PlacedMonster& monster);
		PlacedMonster* GetSelectedMonster();
		entt::registry& GetMonsterRegistry();
		int GetNextWaveIndex() const;
		void SwitchWave(int direction);
		void SendWaveData();
		void UpdateRenderingForCurrentWave();
		std::vector<MonsterWave*> GetWaves();
		void AddWaveWithMonster(std::vector<PlacedMonster> monster);

	private:
		entt::registry m_registry;
		entt::entity m_selectedEntity = entt::null;
		MonsterTypeDefinition m_selectedMonsterItem;
		entt::entity m_currentMonsterWave = entt::null;
		std::vector<entt::entity> m_waveEntities;
	};

}


