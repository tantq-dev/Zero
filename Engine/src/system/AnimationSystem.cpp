#include "ResourcesManager.h"
#include "AnimationSystem.h"

namespace System
{
	void AnimationSystem::Update(entt::registry& registry, const float& dt, class ResourcesManager& resources)
	{
		auto animGroup = registry.group<>(entt::get<Components::Animation, Components::Sprite>);
		for (auto& entity : animGroup)
		{
			auto& anim = registry.get<Components::Animation>(entity);
			auto& sprite = registry.get<Components::Sprite>(entity);

			if (!anim.isPlaying) continue;

			// Handle SpriteAtlas animation
			if (anim.atlasId != 0 && !anim.currentAnimationName.empty())
			{
				const auto* atlas = resources.GetSpriteAtlas(anim.atlasId);
				if (atlas)
				{
					sprite.texture = atlas->texture;
					auto it = atlas->animations.find(anim.currentAnimationName);
					if (it != atlas->animations.end())
					{
						const auto& clip = it->second;
						anim.currentFrameTime += dt;

						if (anim.currentFrameTime >= clip.frameDuration)
						{
							anim.currentFrameTime = 0;
							size_t nextFrameIdx = anim.currentFrameIndex + 1;

							if (nextFrameIdx >= clip.frames.size())
							{
								if (clip.loop)
								{
									anim.currentFrameIndex = 0;
								}
								else
								{
									anim.isFinished = true;
									anim.currentFrameIndex = clip.frames.size() - 1;
								}
							}
							else
							{
								anim.currentFrameIndex = nextFrameIdx;
							}
							size_t currentFrame = it->second.frames[anim.currentFrameIndex];
							sprite.source = atlas->frames[currentFrame];
						}
						continue; // Done with this entity
					}
				}
			}

			
		}
	}

}
