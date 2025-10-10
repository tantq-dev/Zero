#include "SDLRenderer2D.h"

SDLRenderer::SDLRenderer(std::shared_ptr<SDL_Renderer>renderer)
{
	m_renderer = renderer;
}

SDLRenderer::~SDLRenderer()
{
	for (auto& [id, texture] : m_textures)
	{
		if (texture.ptr)
		{
			SDL_DestroyTexture(texture.ptr);
		}
	}
	m_textures.clear();
	m_renderer = nullptr;
}

void SDLRenderer::CallRender()
{
	// Render all sprites in the render queue
	for (auto& [layer, renderDataVec] : m_RenderQueue)
	{
		for (const auto& data : renderDataVec)
		{
			if (data.texture && data.dstrect)
			{
				SDL_RenderTextureRotated(m_renderer.get(), data.texture, data.srcrect, data.dstrect,data.angle, data.center, data.flip);
			
			}
		}
	}
	m_RenderQueue.clear();
}

void SDLRenderer::PushSpriteToRenderQueue(const Components::Sprite& sprite, const Components::Transform2D& transform)
{
    if (!sprite.visible || sprite.opacity <= 0.f || sprite.texture.id == 0) return;
    auto it = m_textures.find(sprite.texture.id);
    if (it == m_textures.end() || !it->second.ptr) return;

    const SDLTextureEntry& t = it->second;
    SDL_FRect src{ sprite.source.x, sprite.source.y, sprite.source.w, sprite.source.h };
	LOG_INFO("Source: " + std::to_string(sprite.source.x));
    SDL_FRect dst{
		transform.position.x - sprite.pivot.x * sprite.source.w * transform.scale.x, // centered on pivot, shift the position by pivot*width*transform.scale ensure scaling is considered
        transform.position.y - sprite.pivot.y * sprite.source.h * transform.scale.y,
        sprite.source.w * transform.scale.x,
        sprite.source.h * transform.scale.y
    };

    // apply color/opacity
    //SDL_SetTextureColorMod(t.ptr,
    //    (Uint8)(255 * sprite.tint.r),
    //    (Uint8)(255 * sprite.tint.g),
    //    (Uint8)(255 * sprite.tint.b));
    //SDL_SetTextureAlphaMod(t.ptr, (Uint8)(255 * (sprite.tint.a * sprite.opacity)));

    const double deg = transform.rotation;
    const SDL_FlipMode flip = sprite.flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	SDL_FPoint center{ sprite.pivot.x * dst.w, sprite.pivot.y * dst.h }; // pivot in pixels
	
	auto layer = m_RenderQueue.find(sprite.layer);
	if (layer != m_RenderQueue.end())
	{
		SDLRendererData data(t.ptr, &src, &dst, deg, &center, flip);
		layer->second.push_back(std::move(data));
	}
	else {
		std::vector<SDLRendererData> data;
		SDLRendererData renderData(t.ptr, &src, &dst, deg, &center, flip);

		data.push_back(renderData);
		m_RenderQueue.insert_or_assign(sprite.layer, std::move(data));
	}

}

Components::Texture SDLRenderer::GetTextureFromFile(const std::string& path)
{
	SDL_Surface* surf = SDL_LoadBMP(path.c_str());
	if (!surf) return Components::Texture{};
	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer.get(), surf);
	Components::Size sz{ (float)surf->w, (float)surf->h };
	SDL_DestroySurface(surf);

	m_nextId++;
	m_textures[m_nextId] = SDLTextureEntry{ {m_nextId,sz}, tex };
	return { m_nextId, sz };
}
