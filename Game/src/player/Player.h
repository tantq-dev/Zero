#pragma once
#include "Vec2.h"
namespace Game
{
	enum class CharacterState
	{
		IDLE,
		MOVING,
		ATTACKING
	};

	enum class CharacterDirection
	{
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	struct Character
	{
		float  moveSpeed;
		bool justSwitchedDirection = false;
		bool justSwitchedState = false;
		CharacterDirection direction = CharacterDirection::NONE;
		CharacterState state = CharacterState::IDLE;
	};

	struct StateAnimation
	{
		std::unordered_map<CharacterState, Components::AnimationClip> stateAnimations;
	};

	struct DirectionAnimation
	{
		std::unordered_map<CharacterDirection, StateAnimation> directionAnimations;
	};
}