#include "AnimationSys.h"
#include <player/Player.h>

void Game::AnimationSys::Update(entt::registry& registry, const float& dt) {
	auto characterAnim = registry.group<>(entt::get<Components::Animation,DirectionAnimation,Character>);
	for (auto& anim : characterAnim) {
		auto& character = registry.get<Character>(anim);
		if (character.justSwitchedDirection && character.justSwitchedState)
		{
			auto& animClip = registry.get<Components::Animation>(anim);
			auto& dirAnim = registry.get<DirectionAnimation>(anim);
			auto& currentAnim = dirAnim.directionAnimations.at(character.direction);
			animClip.currentClip = currentAnim.stateAnimations.at(character.state);
		}
	}
}