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
		void AddMonsterToMap(Vec2 clickPosition);
		void DeleteFromMap(Vec2 clickPosition);
		void OnMonsterSelectedFromUI(MonsterItem monster);
		void ClickOnMap(Vec2 clickPosition);
		void ClickOnPlacedMonster(entt::entity& e, PlacedMonster& monster);
		PlacedMonster* GetSelectedMonster();
		entt::registry& GetMonsterRegistry();
		int GetNextWaveIndex() const;
		void SwitchWave(int direction);
		void SendWaveData();
		void UpdateRenderingForCurrentWave();

	private:
		entt::registry m_registry;
		entt::entity m_selectedEntity = entt::null;
		MonsterItem m_selectedMonsterItem{ -1, "" };
		entt::entity m_currentMonsterWave = entt::null;
		std::vector<entt::entity> m_waveEntities;
	};

}


