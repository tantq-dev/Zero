#pragma once
#include "MonsterModel.h"
#include "utilities/Vec2.h"
#include "entt.hpp"

namespace Tool
{
	class MapEditor
	{
	public:
		MapEditor();
		~MapEditor();
		void SelecteMonsterAtPosition(Vec2 clickPosition);
		void AddMonsterToMap(Vec2 clickPosition);
		void DeleteFromMap(Vec2 clickPosition);
		void OnMonsterSelectedFromUI(MonsterItem monster);
		void ClickOnMap(Vec2 clickPosition);
		void ClickOnPlacedMonster(entt::entity& e, PlacedMonster& monster);
		PlacedMonster* GetSelectedMonster();
		entt::registry& GetMonsterRegistry();


		// New methods for MonsterProperties integration
		MonsterProperties* GetSelectedMonsterProperties();
		MonsterProperties* GetMonsterProperties(entt::entity entity);
		void UpdateMonsterPropertiesFromUI(const MonsterProperties& properties);

	private:
		entt::registry m_registry;
		entt::entity m_selectedEntity = entt::null;
		MonsterItem m_selectedMonsterItem{ -1, "" };
	};

}


