#include "MapEditor.h"
#include "EventKey.h"
#include "core/Components.h"
#include "core/EventSystem.h"
#include "resources/ResourcesManager.h"


Tool::MapEditor::MapEditor()
	: m_selectedEntity(entt::null)
{
	AddWave();
	Core::EventSystem::getInstance().subscribe(EventKeys::MonsterSelectedFromPalette,
		[this](const Core::EventData& data) {
			OnMonsterSelectedFromUI(data.get<MonsterItem>());
		});

	Core::EventSystem::getInstance().subscribe(EventKeys::SwitchWave,
		[this](const Core::EventData& data) {
			SwitchWave(data.get<int>());
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
	SendWaveData();
}

Tool::MapEditor::~MapEditor()
{
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
		auto& currentWave = m_registry.get<MonsterWave>(m_currentMonsterWave);
		currentWave.addMonster(entity);
	}
}


void Tool::MapEditor::OnMonsterSelectedFromUI(MonsterItem monsterItem)
{
	m_selectedMonsterItem = monsterItem;
}

void Tool::MapEditor::ClickOnMap(Vec2 clickPosition)
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


int Tool::MapEditor::GetNextWaveIndex() const
{
	int maxIndex = -1;


	for (auto entity : m_waveEntities) {
		const auto& wave = m_registry.get<MonsterWave>(entity);
		maxIndex = std::fmax(maxIndex, wave.waveIndex);
	}

	return maxIndex + 1;
}

void Tool::MapEditor::SwitchWave(int direction)
{
	if (m_waveEntities.empty())
		return;

	if (m_currentMonsterWave == entt::null) {
		// If no wave is selected, select the first or last one based on direction
		m_currentMonsterWave = direction > 0 ? m_waveEntities.front() : m_waveEntities.back();
		return;
	}

	// Find current wave index in our sorted array
	auto it = std::find(m_waveEntities.begin(), m_waveEntities.end(), m_currentMonsterWave);
	if (it == m_waveEntities.end())
		return;

	// Calculate new index with bounds checking
	int currentIdx = std::distance(m_waveEntities.begin(), it);
	int newIdx = currentIdx + direction;

	// Wrap around or clamp as needed
	if (newIdx < 0)
	{
		newIdx = m_waveEntities.size() - 1;  // Wrap to end
	}
	else if (newIdx >= m_waveEntities.size())
	{
		AddWave();
	}

	m_currentMonsterWave = m_waveEntities[newIdx];

	// Notify that wave has changed
	if (m_registry.valid(m_currentMonsterWave)) {
		SendWaveData();
	}
	//Set visible for all 
	UpdateRenderingForCurrentWave();
}

void Tool::MapEditor::SendWaveData()
{
	Core::EventData eventData;
	const MonsterWave& wave = m_registry.get<MonsterWave>(m_currentMonsterWave);
	const WaveInformation waveInfo(wave.waveIndex, wave.waveIndex == m_waveEntities.size() - 1, wave.isBossWave);
	eventData.data = waveInfo;
	Core::EventSystem::getInstance().publish(EventKeys::SendWaves, eventData);
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








