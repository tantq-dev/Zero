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
						}

						// Update sprite
						if (anim.currentFrameIndex < clip.frames.size())
						{
							sprite.spriteSheetId = 0; // Using atlas instead
							sprite.frameIndex = clip.frames[anim.currentFrameIndex];
							sprite.texture = atlas->texture;
							
							// If the sprite has a source rect, we might want to update it
							if (sprite.frameIndex < atlas->frames.size())
							{
								sprite.source = atlas->frames[sprite.frameIndex];
							}
						}
						continue; // Done with this entity
					}
				}
			}

			// Fallback: Handle legacy AnimationClip logic
			if (anim.currentClip.numberOfFrames > 0)
			{
				anim.currentFrameTime += dt;
				if (anim.currentFrameTime >= anim.currentClip.frameTime)
				{
					anim.currentFrameTime = 0;
					size_t nextFrame = anim.currentFrame + 1;

					if (nextFrame >= anim.currentClip.numberOfFrames)
					{
						if (anim.currentClip.isLoop)
						{
							anim.currentFrame = 0;
						}
						else
						{
							anim.isFinished = true;
							anim.currentFrame = anim.currentClip.numberOfFrames - 1;
						}
					}
					else
					{
						anim.currentFrame = nextFrame;
					}

					sprite.frameIndex = anim.currentClip.frameIndexStart + anim.currentFrame;
					sprite.spriteSheetId = anim.currentClip.spriteSheetId;
				}
			}
		}
	}

}
