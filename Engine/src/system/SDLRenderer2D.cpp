#include "SDLRenderer2D.h"

SDLRenderer::SDLRenderer(std::shared_ptr<SDL_Renderer> renderer)
    : m_renderer(std::move(renderer))
{
}

SDLRenderer::~SDLRenderer()
{
    for (auto& [id, texture] : m_textures)
    {
        if (texture.ptr)
        {
            SDL_DestroyTexture(texture.ptr);
            texture.ptr = nullptr;
        }
    }
    m_textures.clear();

    for (auto& [id, font] : m_fonts)
    {
        if (font)
        {
            TTF_CloseFont(font);
        }
    }
    m_fonts.clear();
}

void SDLRenderer::CallRender()
{
    // Render sprites sorted by layer
    for (const auto& [layer, renderDataVec] : m_RenderQueue)
    {
        for (const auto& data : renderDataVec)
        {
            if (data.texture)
            {
                SDL_RenderTextureRotated(
                    m_renderer.get(),
                    data.texture,           
                    &data.srcRect,
                    &data.dstRect,
                    data.angle,
                    &data.center,
                    data.flip
                );
            }
        }
    }
    m_RenderQueue.clear();
}

void SDLRenderer::PushSpriteToRenderQueue(const Components::Sprite& sprite, const Components::Transform2D& transform)
{
    // Early returns for invalid sprites
    if (!sprite.visible || sprite.opacity <= 0.0f || sprite.texture.id == 0)
    {
        return;
    }

    // Find texture
    auto it = m_textures.find(sprite.texture.id);
    if (it == m_textures.end() || !it->second.ptr)
    {
        return;
    }

    const SDLTextureEntry& textureEntry = it->second;

    // Calculate source rectangle
    const SDL_FRect srcRect{
        sprite.source.x,
        sprite.source.y,
        sprite.source.w,
        sprite.source.h
    };

    // Calculate destination rectangle with pivot offset
    const SDL_FRect dstRect{
        transform.position.x - (sprite.pivot.x * sprite.source.w * transform.scale.x),
        transform.position.y - (sprite.pivot.y * sprite.source.h * transform.scale.y),
        sprite.source.w * transform.scale.x,
        sprite.source.h * transform.scale.y
    };

    // Calculate rotation center in destination rect space
    const SDL_FPoint center{
        sprite.pivot.x * dstRect.w,
        sprite.pivot.y * dstRect.h
    };

    // Determine flip mode
    const SDL_FlipMode flip = sprite.flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // Create render data (stores values, not pointers)
    SDLRendererData renderData{
        textureEntry.ptr,
        srcRect,
        dstRect,
        transform.rotation,
        center,
        flip
    };

    // Add to appropriate layer in render queue
    m_RenderQueue[sprite.layer].push_back(std::move(renderData));
}

Components::Texture SDLRenderer::GetTextureFromFile(const std::string& path)
{
    // Load surface
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (!surface)
    {
        LOG_ERROR("Failed to load texture: " + path);
        return Components::Texture{};
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer.get(), surface);
    
    // Store size before destroying surface
    const Components::Size size{
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };
    
    SDL_DestroySurface(surface);

    if (!texture)
    {
        LOG_ERROR("Failed to create texture from surface: " + path);
        return Components::Texture{};
    }

    // Store texture with unique ID
    ++m_nextId;
    m_textures[m_nextId] = SDLTextureEntry{
        { m_nextId, size },
        texture
    };

    return { m_nextId, size };
}

uint32_t SDLRenderer::LoadFont(const std::string& path, float size)
{
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font)
    {
        LOG_ERROR("Failed to load font: " + path + " - " + std::string(SDL_GetError()));
        return 0;
    }

    uint32_t fontId = ++m_nextFontId;
    m_fonts[fontId] = font;
    return fontId;
}

void SDLRenderer::UnloadFont(uint32_t fontId)
{
    auto it = m_fonts.find(fontId);
    if (it != m_fonts.end())
    {
        TTF_CloseFont(it->second);
        m_fonts.erase(it);
    }
}

uint32_t SDLRenderer::RenderTextToTexture(uint32_t fontId, const std::string& text, SDL_Color color, bool wordWrap, int wrapWidth, float& outWidth, float& outHeight)
{
    auto it = m_fonts.find(fontId);
    if (it == m_fonts.end() || !it->second)
    {
        return 0;
    }

    SDL_Surface* surface = nullptr;
    if (wordWrap && wrapWidth > 0)
    {
        surface = TTF_RenderText_Blended_Wrapped(it->second, text.c_str(), 0, color, wrapWidth);
    }
    else
    {
        surface = TTF_RenderText_Blended(it->second, text.c_str(), 0, color);
    }

    if (!surface)
    {
        LOG_ERROR("Failed to render text surface: " + std::string(SDL_GetError()));
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer.get(), surface);
    outWidth = static_cast<float>(surface->w);
    outHeight = static_cast<float>(surface->h);
    SDL_DestroySurface(surface);

    if (!texture)
    {
        LOG_ERROR("Failed to create texture from text surface: " + std::string(SDL_GetError()));
        return 0;
    }

    uint32_t textureId = ++m_nextId;
    m_textures[textureId] = SDLTextureEntry{
        { textureId, { outWidth, outHeight } },
        texture
    };

    return textureId;
}

void SDLRenderer::PushTextToRenderQueue(uint32_t textureId, float width, float height, const Components::Transform2D& transform, int layer, Components::TextAlign align)
{
    auto it = m_textures.find(textureId);
    if (it == m_textures.end() || !it->second.ptr)
    {
        return;
    }

    // Source rect is the whole texture
    const SDL_FRect srcRect{ 0.0f, 0.0f, width, height };

    // Calculate pivot based on alignment
    float pivotX = 0.5f; // Default center
    if (align == Components::TextAlign::Left) pivotX = 0.0f;
    else if (align == Components::TextAlign::Right) pivotX = 1.0f;

    const SDL_FRect dstRect{
        transform.position.x - (pivotX * width * transform.scale.x),
        transform.position.y - (0.5f * height * transform.scale.y), // Vertical center for simplicity
        width * transform.scale.x,
        height * transform.scale.y
    };

    const SDL_FPoint center{ pivotX * dstRect.w, 0.5f * dstRect.h };

    SDLRendererData renderData{
        it->second.ptr,
        srcRect,
        dstRect,
        transform.rotation,
        center,
        SDL_FLIP_NONE
    };

    m_RenderQueue[layer].push_back(std::move(renderData));
}
