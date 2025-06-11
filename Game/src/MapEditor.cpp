#include "MapEditor.h"
#include "EventKey.h"
#include "core/Components.h"
#include "core/EventSystem.h"
#include "resources/ResourcesManager.h"


Tool::MapEditor::MapEditor()
	: m_selectedEntity(entt::null)
{
	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromPalate,
		[this](const Core::EventData& data) {
			OnMonsterSelectedFromUI(data.get<MonsterItem>());
		});
}

Tool::MapEditor::~MapEditor()
{
}

void Tool::MapEditor::SelecteMonsterAtPosition(Vec2 clickPosition)
{
	m_selectedEntity = entt::null;

	auto view = m_registry.group<>(entt::get<PlacedMonster, Components::Transform>);
	for (auto entity : view) {
		auto [monster, transform] = view.get<PlacedMonster, Components::Transform>(entity);
		if (true) { //todo check condition to return monster at position
			m_selectedEntity = entity;
			break;
		}
	}
}

void Tool::MapEditor::AddMonsterToMap(Vec2 clickPosition)
{
	if (m_selectedMonsterItem.id != -1) {
		auto entity = m_registry.create();
		m_registry.emplace<PlacedMonster>(entity, m_selectedMonsterItem);
		m_registry.emplace<Components::Transform>(entity, clickPosition, Vec2{ 10,10 });
		m_registry.emplace<Components::Sprite>(entity, ResourcesManager::GetInstance().GetTexture(m_selectedMonsterItem.name));
		m_selectedEntity = entity;
		m_registry.get<PlacedMonster>(entity).properties.name = m_selectedMonsterItem.name;
	}
}


void Tool::MapEditor::OnMonsterSelectedFromUI(MonsterItem monsterItem)
{
	m_selectedMonsterItem = monsterItem;
}

void Tool::MapEditor::ClickOnMap(Vec2 clickPosition)
{
	auto group = m_registry.group<>(entt::get<Components::Transform, PlacedMonster>);

	for (auto entity : group) {
		auto transform = group.get<Components::Transform>(entity);
		if (transform.position == clickPosition)
		{
			PlacedMonster& monster = group.get<PlacedMonster>(entity);
			ClickOnPlacedMonster(entity, monster);
			return;
		}
	}
	// no monster in this place
	AddMonsterToMap(clickPosition);


}
void Tool::MapEditor::ClickOnPlacedMonster(entt::entity& monsterEntity, PlacedMonster& monster) {
	m_selectedEntity = monsterEntity;
	Core::EventData eventData;
	eventData.data = &monster;  // Send the entire monster item
	Core::EventSystem::getInstance().publish(EventKeys::MonsterSelectedFromMap, eventData);

}

// Add this helper method to find the selected monster
PlacedMonster* Tool::MapEditor::GetSelectedMonster()
{
	if (m_selectedEntity != entt::null && m_registry.valid(m_selectedEntity)) {
		return &m_registry.get<PlacedMonster>(m_selectedEntity);
	}
	return nullptr;
}

entt::registry& Tool::MapEditor::GetMonsterRegistry()
{
	return m_registry;
}


void Tool::MapEditor::DeleteFromMap(Vec2 clickPosition)
{
	entt::entity entityToDelete = entt::null;

	auto group = m_registry.group<>(entt::get<Components::Transform, PlacedMonster>);

	for (auto entity : group) {
		auto transform = group.get<Components::Transform>(entity);
		if (transform.position == clickPosition)
		{
			entityToDelete = entity;
			break; // Exit the loop once we find the entity to delete
		}
	}

	// Delete the entity outside the loop if one was found
	if (entityToDelete != entt::null && m_registry.valid(entityToDelete)) {
		m_registry.destroy(entityToDelete);
		// Clear selection if we deleted the selected entity
		if (entityToDelete == m_selectedEntity) {
			m_selectedEntity = entt::null;
		}
	}
}








