#include "Components.h"
#include "AnimationSystem.h"

namespace System
{
	void AnimationSystem::Update(entt::registry& registry, const float& dt)
	{
		auto animGroup = registry.group<>(entt::get<Components::Animation,Components::Sprite>);
		for (auto& anim : animGroup)
		{
			auto& animClip = registry.get<Components::Animation>(anim);
			auto& sprite = registry.get<Components::Sprite>(anim);
			animClip.currentFrameTime += dt;
			if (animClip.currentFrameTime > animClip.currentClip.frameTime)
			{
				animClip.currentFrameTime = 0;
				animClip.currentFrame += 1;
				animClip.currentFrame %= animClip.currentClip.spriteSheet.frames.size();
				sprite.texture = animClip.currentClip.spriteSheet.texture;
				sprite.source = animClip.currentClip.spriteSheet.frames[animClip.currentFrame];

			}
		}
	}

}
