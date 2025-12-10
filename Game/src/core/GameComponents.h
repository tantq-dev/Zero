#pragma once
#include "Vec2.h"

struct Health
{
	int current = 100;
	int max = 100;
};

struct Enemy
{
	float detectRange = 200.0f;
	float attackRange = 30.0f;
	float moveSpeed = 50.0f;
	bool isChasing = false;
};

struct MeleeAttack
{
	int damage = 10;
	float duration = 0.3f; // Seconds
	float cooldown = 0.5f;
	float timeActive = 0.0f;
	float timeSinceLastAttack = 0.0f;
	bool isActive = false;
};
