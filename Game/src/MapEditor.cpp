#include "MapEditor.h"
#include "EventKey.h"
#include "core/Components.h"
#include "core/EventSystem.h"
#include "resources/ResourcesManager.h"


Tool::MapEditor::MapEditor()
	: m_selectedEntity(entt::null)
{
	AddWave();
	SendWaveData();
	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromPalette,
		[this](const Core::EventData& data) {
			OnMonsterSelectedFromUI(data.get<MonsterTypeDefinition>());
		});

	Core::EventSystem::getInstance().subscribe(EventKeys::SwitchWave,
		[this](const Core::EventData& data) {
			SwitchWave(data.get<int>());
		});
	Core::EventSystem::getInstance().subscribe(EventKeys::AddWave,
		[this](const Core::EventData& data) {
			AddWave();
			SendWaveData();
		});
}

void Tool::MapEditor::AddWave()
{
	auto entity = m_registry.create();
	m_registry.emplace<MonsterWave>(entity);
	m_currentMonsterWave = entity;
	MonsterWave& newWave = m_registry.get<MonsterWave>(entity);
	newWave.waveIndex = GetNextWaveIndex();
	m_waveEntities.push_back(entity);
	// Sort waves by index for proper sequence
	std::sort(m_waveEntities.begin(), m_waveEntities.end(),
		[this](entt::entity a, entt::entity b) {
			return m_registry.get<MonsterWave>(a).waveIndex <
				m_registry.get<MonsterWave>(b).waveIndex;
		});
}

Tool::MapEditor::~MapEditor()
{
}


void Tool::MapEditor::AddMonsterToMap(Vec2 clickPosition, Vec2 gridPosition)
{
	if (m_selectedMonsterItem.item.id != -1) {
		LOG_INFO("Click at index: " + std::to_string(gridPosition.x) + " " + std::to_string(gridPosition.y));
		auto entity = m_registry.create();
		m_registry.emplace<PlacedMonster>(entity, m_selectedMonsterItem.item);
		m_registry.emplace<Components::Transform>(entity, clickPosition, Vec2{ 10,10 });
		m_registry.emplace<Components::Sprite>(entity, ResourcesManager::GetInstance().GetTexture(m_selectedMonsterItem.textureName));
		m_selectedEntity = entity;
		m_registry.get<PlacedMonster>(entity).properties.name = m_selectedMonsterItem.item.name;
		m_registry.get<PlacedMonster>(entity).positionInGrid = gridPosition;
		m_registry.get<PlacedMonster>(entity).worldPosition = clickPosition;



		auto& currentWave = m_registry.get<MonsterWave>(m_currentMonsterWave);
		currentWave.addMonster(entity);
	}
}


void Tool::MapEditor::OnMonsterSelectedFromUI(MonsterTypeDefinition monsterItem)
{
	m_selectedMonsterItem = monsterItem;
}

void Tool::MapEditor::ClickOnMap(Vec2 clickPosition, Vec2 gridPosition)
{
	auto& currentWave = m_registry.get<MonsterWave>(m_currentMonsterWave);


	for (auto entity : currentWave.monsterEntities) {
		auto transform = m_registry.get<Components::Transform>(entity);
		if (transform.position == clickPosition)
		{
			PlacedMonster& monster = m_registry.get<PlacedMonster>(entity);
			//ClickOnPlacedMonster(entity, monster);
			return;
		}
	}
	// no monster in this place
	AddMonsterToMap(clickPosition, gridPosition);


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


int Tool::MapEditor::GetNextWaveIndex() const
{
	int maxIndex = -1;


	for (auto entity : m_waveEntities) {
		const auto& wave = m_registry.get<MonsterWave>(entity);
		maxIndex = std::fmax(maxIndex, wave.waveIndex);
	}

	return maxIndex + 1;
}

void Tool::MapEditor::SwitchWave(int index)
{
	if (m_waveEntities.empty())
		return;
	m_currentMonsterWave = m_waveEntities[index];

	// Notify that wave has changed
	if (m_registry.valid(m_currentMonsterWave)) {
		SendWaveData();
	}
	//Set visible for all 
	UpdateRenderingForCurrentWave();
}

void Tool::MapEditor::SendWaveData()
{
	Core::EventData waveEventData;
	std::vector<MonsterWave*> monsterWave;
	for (entt::entity& wave : m_waveEntities) {
		monsterWave.push_back(&m_registry.get<MonsterWave>(wave));
	}
	waveEventData.data = monsterWave;

	const MonsterWave& wave = m_registry.get<MonsterWave>(m_currentMonsterWave);
	Core::EventData waveIndexData;
	waveIndexData.data = wave.waveIndex;

	Core::EventSystem::getInstance().publish(EventKeys::SendWaves, waveEventData);
	Core::EventSystem::getInstance().publish(EventKeys::UISwitchWave, waveIndexData);

}

// Add this method to handle rendering logic when switching waves
void Tool::MapEditor::UpdateRenderingForCurrentWave()
{
	// First, make all monsters invisible
	auto allMonsters = m_registry.group(entt::get<PlacedMonster, Components::Sprite>);
	for (auto entity : allMonsters) {
		auto& sprite = m_registry.get<Components::Sprite>(entity);
		sprite.isVisible = false;  // Assuming your Sprite component has a visible flag
	}

	// Then, make only current wave monsters visible
	if (m_registry.valid(m_currentMonsterWave)) {
		auto& currentWave = m_registry.get<MonsterWave>(m_currentMonsterWave);
		for (auto entity : currentWave.monsterEntities) {
			if (m_registry.valid(entity) && m_registry.all_of<Components::Sprite>(entity)) {
				auto& sprite = m_registry.get<Components::Sprite>(entity);
				sprite.isVisible = true;
			}
		}
	}
}

std::vector<MonsterWave*> Tool::MapEditor::GetWaves()
{
	std::vector<MonsterWave*> waves;
	for (auto& wave : m_waveEntities) {
		waves.push_back(&m_registry.get<MonsterWave>(wave));
	}
	return waves;
}

void Tool::MapEditor::AddWaveWithMonster(std::vector<PlacedMonster> monsters)
{
	AddWave();
	for (auto& monster : monsters) {
		m_selectedMonsterItem.item.id = monster.item.id;
		m_selectedMonsterItem.textureName = monster.properties.valideMonsterIngame;
		AddMonsterToMap(monster.worldPosition, monster.positionInGrid);
	}
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
		MonsterWave& wave = m_registry.get<MonsterWave>(m_currentMonsterWave);
		wave.removeMonster(entityToDelete);
		m_registry.destroy(entityToDelete);
		// Clear selection if we deleted the selected entity
		if (entityToDelete == m_selectedEntity) {
			m_selectedEntity = entt::null;
		}
	}
}








