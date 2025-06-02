#include "Components.h"
#include "AnimationSystem.h"

namespace System
{
	void AnimationSystem::Update(entt::registry& registry, const float& dt)
	{
		auto group = registry.group<>(entt::get<Components::Animator>);

		for (auto &entity: group)
		{
			auto &animator = group.get<Components::Animator>(entity);
			Components::Animation* animation = animator.GetCurrentAnimation();
			animation->currentTime+=dt;
			animation->currentTime = fmod(animation->currentTime, animation->frameCount*animation->speed);
			animation->currentFrame = round(animation->currentTime / animation->speed);
			if (animation->currentFrame * animation->frameWidth >= animation->texture->w)
			{
				animation->currentFrame = 0;
			}
		}
	}

}
