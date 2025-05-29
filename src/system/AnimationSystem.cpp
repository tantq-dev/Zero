#include "AnimationSystem.h"
namespace System
{
	void AnimationSystem::Update(entt::registry& registry, const float& dt)
	{
		auto group = registry.group<>(entt::get<Components::Animation, Components::Sprite>);

		for (auto &entity: group)
		{
			auto& animation = group.get<Components::Animation>(entity);
			auto& sprite = group.get<Components::Sprite>(entity);

			animation.currentTime+=dt;
			animation.currentTime = fmod(animation.currentTime, animation.frameCount*animation.speed);
			animation.currentFrame = round(animation.currentTime / animation.speed);
			if (animation.currentFrame * animation.frameWidth >= sprite.texture->w)
			{
				animation.currentFrame = 0;
			}
		}
	}

}
