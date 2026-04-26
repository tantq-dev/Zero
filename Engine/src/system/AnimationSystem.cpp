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
			if (animClip.currentFrameTime >= animClip.currentClip.frameTime)
			{
				animClip.currentFrameTime = 0;
				size_t nextFrame = animClip.currentFrame + 1;
				
				if (nextFrame >= animClip.currentClip.numberOfFrames)
				{
					if (animClip.currentClip.isLoop)
					{
						animClip.currentFrame = 0;
					}
					else
					{
						animClip.isFinished = true;
						// Stay on last frame if not looping
						animClip.currentFrame = animClip.currentClip.numberOfFrames - 1;
					}
				}
				else
				{
					animClip.currentFrame = nextFrame;
				}

				sprite.frameIndex = animClip.currentClip.frameIndexStart + animClip.currentFrame;
				sprite.spriteSheetId = animClip.currentClip.spriteSheetId;
			}
		}
	}

}
