#pragma once
#include "RenderComponents.h"
#include "json.hpp"
#include <string>
#include <fstream>

namespace Utilities
{
	class SpriteAtlasLoader
	{
	public:
		static Components::SpriteAtlas LoadFromJson(const std::string& jsonContent, const Components::Texture& texture)
		{
			Components::SpriteAtlas atlas;
			atlas.texture = texture;

			try
			{
				auto data = nlohmann::json::parse(jsonContent);

				// Parse frames
				if (data.contains("frames") && data["frames"].is_array())
				{
					for (const auto& frameData : data["frames"])
					{
						Components::Rect rect;
						rect.x = frameData.value("x", 0.0f);
						rect.y = frameData.value("y", 0.0f);
						rect.w = frameData.value("w", 0.0f);
						rect.h = frameData.value("h", 0.0f);
						atlas.frames.push_back(rect);
					}
				}

				// Parse animations
				if (data.contains("animations") && data["animations"].is_object())
				{
					for (auto& [name, animData] : data["animations"].items())
					{
						Components::SpriteAtlasAnimation anim;
						anim.loop = animData.value("loop", true);
						anim.frameDuration = animData.value("frameDuration", 0.1f);
						
						if (animData.contains("frames") && animData["frames"].is_array())
						{
							for (const auto& frameIdx : animData["frames"])
							{
								anim.frames.push_back(frameIdx.get<size_t>());
							}
						}
						
						atlas.animations[name] = std::move(anim);
					}
				}
			}
			catch (const std::exception& e)
			{
				// Log error or handle gracefully
			}

			return atlas;
		}
	};
}
