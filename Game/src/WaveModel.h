#pragma once
#include "MonsterModel.h"
#include <vector>
#include"entt.hpp"

struct MonsterWave {
	std::vector<entt::entity> monsterEntities;
	bool isBossWave = false;
	int waveIndex = 0;

	void addMonster(entt::entity entity) {
		monsterEntities.push_back(entity);
	}

	void removeMonster(entt::entity entity) {
		auto it = std::find(monsterEntities.begin(), monsterEntities.end(), entity);
		if (it != monsterEntities.end()) {
			monsterEntities.erase(it);
		}
	}
};

struct WaveInformation {
	int waveIndex;
	bool isLastWave;
	bool isBossWave;
};
