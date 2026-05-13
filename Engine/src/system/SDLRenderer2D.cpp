#include "SDLRenderer2D.h"
#include <set>
#define _USE_MATH_DEFINES
#include <Game.h>
#include "stb_image.h"
#undef min

SDL_Texture* SDLRenderer2D::CreateWhiteTexture()
{
    Uint32 pixel = 0xFFFFFFFF; // RGBA white

    SDL_Texture* tex = SDL_CreateTexture(
        m_renderer.get(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,
        1, 1
    );

    SDL_UpdateTexture(tex, nullptr, &pixel, sizeof(Uint32));
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    return tex;
}

SDLRenderer2D::SDLRenderer2D(std::shared_ptr<SDL_Renderer> renderer)
    : m_renderer(std::move(renderer))
{
    SDL_SetRenderLogicalPresentation(m_renderer.get(), m_virtualWidth, m_virtualHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    m_whiteTexture = CreateWhiteTexture();

    // Compute UI scale: ratio of real output pixels to virtual resolution.
    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(m_renderer.get(), &outW, &outH);
    if (outW > 0 && outH > 0)
    {
        float sx = static_cast<float>(outW) / static_cast<float>(m_virtualWidth);
        float sy = static_cast<float>(outH) / static_cast<float>(m_virtualHeight);
        m_uiScale = std::min(sx, sy);
    }
}

SDLRenderer2D::~SDLRenderer2D()
{
    for (auto& [id, texture] : m_textures) {
        SDL_DestroyTexture(texture.ptr);
    }

    for (auto& [id, font] : m_fonts) {
        TTF_CloseFont(font);
    }
}

void SDLRenderer2D::BeginFrame()
{
    SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(m_renderer.get());
}

void SDLRenderer2D::EndFrame(int /*windowW*/, int /*windowH*/)
{
    // ---- Flush UI screen-space queues (drawn on top of world) ----

    // Disable logical presentation temporarily so we can draw in virtual coords directly.
    // (SDL3 logical presentation keeps virtual coords active, so we can just draw normally.)

    // 1. UI filled/outline rects
    for (const auto& entry : m_uiRects)
    {
        SDL_FColor c = {
            entry.color.r / 255.0f,
            entry.color.g / 255.0f,
            entry.color.b / 255.0f,
            entry.color.a / 255.0f
        };
        if (entry.fill)
        {
            AddColoredQuad(m_uiWhiteBatch, entry.rect, c);
        }
        else
        {
            // Outline: four thin quads
            float t = 1.0f;
            SDL_FRect top    = { entry.rect.x, entry.rect.y,                  entry.rect.w, t };
            SDL_FRect bottom = { entry.rect.x, entry.rect.y + entry.rect.h - t, entry.rect.w, t };
            SDL_FRect left   = { entry.rect.x, entry.rect.y,                  t, entry.rect.h };
            SDL_FRect right  = { entry.rect.x + entry.rect.w - t, entry.rect.y, t, entry.rect.h };
            AddColoredQuad(m_uiWhiteBatch, top,    c);
            AddColoredQuad(m_uiWhiteBatch, bottom, c);
            AddColoredQuad(m_uiWhiteBatch, left,   c);
            AddColoredQuad(m_uiWhiteBatch, right,  c);
        }
    }

    // Flush the white-texture batch for UI rects.
    if (!m_uiWhiteBatch.vertices.empty())
    {
        m_uiWhiteBatch.texture = m_whiteTexture;
        SDL_RenderGeometry(
            m_renderer.get(),
            m_whiteTexture,
            m_uiWhiteBatch.vertices.data(), (int)m_uiWhiteBatch.vertices.size(),
            m_uiWhiteBatch.indices.data(),  (int)m_uiWhiteBatch.indices.size()
        );
        m_uiWhiteBatch.vertices.clear();
        m_uiWhiteBatch.indices.clear();
    }

    // 2. UI sprite images
    for (const auto& entry : m_uiSprites)
    {
        if (!entry.tex) continue;
        SDL_FRect src = { 0, 0, entry.dst.w, entry.dst.h };
        SDL_RenderTexture(m_renderer.get(), entry.tex, &src, &entry.dst);
    }

    // 3. UI text glyphs — render at native resolution for crisp text.
    //    Temporarily disable logical presentation so we draw in real pixels.
    if (!m_uiTexts.empty())
    {
        SDL_SetRenderLogicalPresentation(m_renderer.get(), 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

        int outW = 0, outH = 0;
        SDL_GetRenderOutputSize(m_renderer.get(), &outW, &outH);

        float sx = static_cast<float>(outW) / static_cast<float>(m_virtualWidth);
        float sy = static_cast<float>(outH) / static_cast<float>(m_virtualHeight);
        float scale = std::min(sx, sy);

        // Letterbox offset so text aligns with the virtual viewport.
        float offsetX = (outW - m_virtualWidth * scale) * 0.5f;
        float offsetY = (outH - m_virtualHeight * scale) * 0.5f;

        for (const auto& entry : m_uiTexts)
        {
            if (!entry.tex) continue;

            // Texture holds real-pixel glyph data — query its actual size.
            float texW = 0.0f, texH = 0.0f;
            SDL_GetTextureSize(entry.tex, &texW, &texH);

            SDL_FRect src = { 0, 0, texW, texH };
            SDL_FRect dst = {
                entry.x * scale + offsetX,
                entry.y * scale + offsetY,
                texW,
                texH
            };
            SDL_RenderTexture(m_renderer.get(), entry.tex, &src, &dst);
        }

        // Restore logical presentation.
        SDL_SetRenderLogicalPresentation(m_renderer.get(), m_virtualWidth, m_virtualHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    }

    // Clear UI queues for next frame.
    m_uiRects.clear();
    m_uiSprites.clear();
    m_uiTexts.clear();

    SDL_RenderPresent(m_renderer.get());
}

void SDLRenderer2D::CallRender()
{
    std::vector<uint32_t> layers;
    layers.reserve(m_Batches.size());

    for (auto& [l, _] : m_Batches)
        layers.push_back(l);

    std::sort(layers.begin(), layers.end());

    for (auto layer : layers)
    {
        auto it = m_Batches.find(layer);
        if (it == m_Batches.end()) continue;

        auto& textureMap = it->second;

        for (auto& [texture, batch] : textureMap)
        {
            if (batch.vertices.empty()) continue;

            SDL_RenderGeometry(
                m_renderer.get(),
                texture,
                batch.vertices.data(),
                (int)batch.vertices.size(),
                batch.indices.data(),
                (int)batch.indices.size()
            );
        }
    }

    // clear batches
    for (auto& [_, texMap] : m_Batches)
    {
        for (auto& [__, batch] : texMap)
        {
            batch.vertices.clear();
            batch.indices.clear();
        }
    }
}


void SDLRenderer2D::PushSpriteToRenderQueue(const Components::Sprite& sprite,
    const Components::Transform2D& transform)
{
    if (!sprite.visible || sprite.texture.id == 0) return;

    auto it = m_textures.find(sprite.texture.id);
    if (it == m_textures.end()) return;
    
    SDL_FRect src{
        sprite.source.x,
        sprite.source.y,
        sprite.source.w,
        sprite.source.h
    };

    SDL_FRect dst{
        transform.position.x - sprite.pivot.x * sprite.source.w * transform.scale.x,
        transform.position.y - sprite.pivot.y * sprite.source.h * transform.scale.y,
        sprite.source.w * transform.scale.x,
        sprite.source.h * transform.scale.y
    };

    SDL_FColor sdlColor = {
        sprite.tint.r / 255.0f,
        sprite.tint.g / 255.0f,
        sprite.tint.b / 255.0f,
        sprite.opacity
    };

    auto& batch = m_Batches[sprite.layer][it->second.ptr];
    batch.texture = it->second.ptr;

    if (batch.vertices.capacity() == 0)
    {
        batch.vertices.reserve(2048);
        batch.indices.reserve(3072);
    }

    AddSpriteToBatch(batch, src, dst, transform, &it->second, sdlColor);
}
Components::Texture SDLRenderer2D::GetTextureFromFile(const std::string& path)
{
    int width, height, channels;

    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!pixels)
    {
        LOG_ERROR("Failed to load texture: " + path);
        return Components::Texture{};
    }

    SDL_Texture* texture = SDL_CreateTexture(
        m_renderer.get(),
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        width,
        height
    );

    if (!texture)
    {
        LOG_ERROR("Failed to create texture: " + path + " Error: " + std::string(SDL_GetError()));
        stbi_image_free(pixels);
        return Components::Texture{};
    }

    SDL_UpdateTexture(texture, nullptr, pixels, width * 4);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    stbi_image_free(pixels);

    const Components::Size size{
        static_cast<float>(width),
        static_cast<float>(height)
    };

    ++m_nextId;
    m_textures[m_nextId] = SDLTextureEntry{
        { m_nextId, size },
        texture
    };

    return { m_nextId, size };
}

uint32_t SDLRenderer2D::LoadFont(const std::string& path, float size)
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

void SDLRenderer2D::UnloadFont(uint32_t fontId)
{
    auto it = m_fonts.find(fontId);
    if (it != m_fonts.end())
    {
        TTF_CloseFont(it->second);
        m_fonts.erase(it);
    }
}

uint32_t SDLRenderer2D::RenderTextToTexture(uint32_t fontId, const std::string& text, Components::Color color, bool wordWrap, int wrapWidth, float& outWidth, float& outHeight)
{
    auto it = m_fonts.find(fontId);
    if (it == m_fonts.end() || !it->second)
    {
        return 0;
    }
    SDL_Color sdlColor = {
        static_cast<Uint8>(color.r),
        static_cast<Uint8>(color.g),
        static_cast<Uint8>(color.b),
        static_cast<Uint8>(color.a)
    };

    SDL_Surface* surface = nullptr;
    if (wordWrap && wrapWidth > 0)
    {
        surface = TTF_RenderText_Solid_Wrapped(it->second, text.c_str(), 0, sdlColor, wrapWidth);
    }
    else
    {
        surface = TTF_RenderText_Solid(it->second, text.c_str(), 0, sdlColor);
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
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);

    uint32_t textureId = ++m_nextId;
    m_textures[textureId] = SDLTextureEntry{
        { textureId, { outWidth, outHeight } },
        texture
    };

    return textureId;
}

void SDLRenderer2D::PushTextToRenderQueue(
    uint32_t textureId,
    float width,
    float height,
    const Components::Transform2D& transform,
    int layer,
    Components::TextAlign align)
{
    auto it = m_textures.find(textureId);
    if (it == m_textures.end() || !it->second.ptr)
        return;

    SDLTextureEntry* texEntry = &it->second;

    // Source = full texture
    SDL_FRect src{
        0.0f, 0.0f,
        width, height
    };

    // Alignment → pivot
    float pivotX = 0.5f;
    if (align == Components::TextAlign::Left) pivotX = 0.0f;
    else if (align == Components::TextAlign::Right) pivotX = 1.0f;

    float pivotY = 0.5f;

    SDL_FRect dst{
        transform.position.x - pivotX * width * transform.scale.x,
        transform.position.y - pivotY * height * transform.scale.y,
        width * transform.scale.x,
        height * transform.scale.y
    };

    dst = ApplyCamera(dst);

    
    auto& batch = m_Batches[layer][texEntry->ptr];
    batch.texture = texEntry->ptr;

    if (batch.vertices.capacity() == 0)
    {
        batch.vertices.reserve(512);
        batch.indices.reserve(768);
    }

    AddSpriteToBatch(batch, src, dst, transform, texEntry, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void SDLRenderer2D::DrawRect(Components::Rect rect, Components::Color color, bool fill, int layer)
{
    SDL_FRect sdlRect = { rect.x,rect.y,rect.w,rect.h };
    SDL_FColor sdlColor = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    sdlRect = ApplyCamera(sdlRect);

    if (!fill)
    {
        // optional: keep outline using 4 thin quads or keep SDL_RenderRect
        return;
    }

    auto& batch = m_Batches[layer][m_whiteTexture];
    batch.texture = m_whiteTexture;

    if (batch.vertices.capacity() == 0)
    {
        batch.vertices.reserve(2048);
        batch.indices.reserve(3072);
    }

    AddColoredQuad(batch, sdlRect, sdlColor);
}

void SDLRenderer2D::DrawLine(float x1, float y1, float x2, float y2, Components::Color color, int layer)
{
    float thickness = 1.0f;

    // Apply Camera
    SDL_FRect p1 = ApplyCamera({ x1, y1, 0, 0 });
    SDL_FRect p2 = ApplyCamera({ x2, y2, 0, 0 });
    float tx1 = p1.x; float ty1 = p1.y;
    float tx2 = p2.x; float ty2 = p2.y;

    float dx = tx2 - tx1;
    float dy = ty2 - ty1;

    float len = sqrt(dx * dx + dy * dy);
    if (len == 0) return;

    dx /= len;
    dy /= len;

    // perpendicular
    float px = -dy * thickness * 0.5f;
    float py = dx * thickness * 0.5f;

    SDL_Vertex v[4];

    v[0].position = { tx1 + px, ty1 + py };
    v[1].position = { tx2 + px, ty2 + py };
    v[2].position = { tx2 - px, ty2 - py };
    v[3].position = { tx1 - px, ty1 - py };

    for (int i = 0; i < 4; i++)
    {
        v[i].tex_coord = { 0,0 };
        v[i].color = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    }

    auto& batch = m_Batches[layer][m_whiteTexture];
    batch.texture = m_whiteTexture;

    int start = (int)batch.vertices.size();

    batch.vertices.insert(batch.vertices.end(), &v[0], &v[4]);

    batch.indices.push_back(start + 0);
    batch.indices.push_back(start + 1);
    batch.indices.push_back(start + 2);

    batch.indices.push_back(start + 0);
    batch.indices.push_back(start + 2);
    batch.indices.push_back(start + 3);
}
Vec2 SDLRenderer2D::ScreenToWorld(Vec2 screenPos)
{
    float lx, ly;
    SDL_RenderCoordinatesFromWindow(m_renderer.get(), screenPos.x, screenPos.y, &lx, &ly);

    Vec2 v;
    v.x = lx;
    v.y = ly;

    // virtual → world (center-based)
    if (m_Camera)
    {
        float halfW = m_virtualWidth * 0.5f;
        float halfH = m_virtualHeight * 0.5f;

        v.x = (v.x - halfW) / m_Camera->zoom + m_Camera->position.x;
        v.y = (v.y - halfH) / m_Camera->zoom + m_Camera->position.y;
    }

    return v;
}

Vec2 SDLRenderer2D::ScreenToLogical(Vec2 screenPos)
{
    float lx, ly;
    SDL_RenderCoordinatesFromWindow(m_renderer.get(), screenPos.x, screenPos.y, &lx, &ly);
    return { lx, ly };
}

SDL_FRect SDLRenderer2D::ApplyCamera(const SDL_FRect& rect)
{
    if (!m_Camera) return rect;

    SDL_FRect out = rect;

    // center of virtual screen
    float halfW = m_virtualWidth * 0.5f;
    float halfH = m_virtualHeight * 0.5f;

    // world → camera (center-based)
    out.x = (rect.x - m_Camera->position.x) * m_Camera->zoom + halfW;
    out.y = (rect.y - m_Camera->position.y) * m_Camera->zoom + halfH;

    out.w *= m_Camera->zoom;
    out.h *= m_Camera->zoom;

    return out;
}

void SDLRenderer2D::AddSpriteToBatch(GeometryBatch& batch, const SDL_FRect& src, const SDL_FRect& dst, const Components::Transform2D& transform, SDLTextureEntry* textureEntry, SDL_FColor color)
{
    int start = (int)batch.vertices.size();

    float texW = textureEntry->meta.size.width;
    float texH = textureEntry->meta.size.height;

    float u0 = src.x / texW;
    float v0 = src.y / texH;
    float u1 = (src.x + src.w) / texW;
    float v1 = (src.y + src.h) / texH;

    SDL_Vertex v[4];

    // 1. Calculate world-space corners relative to the pivot
    // The 'dst' passed in already has the pivot offset applied:
    // dst.x = transform.position.x - pivot.x * (src.w * scale.x)
    // We want to recover the local offsets from transform.position.x/y
    float worldW = src.w * transform.scale.x;
    float worldH = src.h * transform.scale.y;

    // Local corners relative to (0,0) before rotation/pivot
    // We need the pivot from the callsite or inferred. 
    // Since we don't have the Sprite here, let's assume the pivot is baked into 'dst'
    // But we need the rotation center. The rotation center is transform.position.
    
    // Actually, let's just use the world position directly.
    float cx = transform.position.x;
    float cy = transform.position.y;
    
    // Recover pivot from dst
    // dst.x = cx - pivotX * worldW  => pivotX = (cx - dst.x) / worldW
    float pX = (worldW != 0) ? (cx - dst.x) / worldW : 0.5f;
    float pY = (worldH != 0) ? (cy - dst.y) / worldH : 0.5f;

    float angleRad = transform.rotation * (3.1415926535f / 180.0f);
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    // World-space corners relative to cx, cy
    float corners[4][2] = {
        { -pX * worldW,             -pY * worldH },
        { (1.0f - pX) * worldW,     -pY * worldH },
        { (1.0f - pX) * worldW,     (1.0f - pY) * worldH },
        { -pX * worldW,             (1.0f - pY) * worldH }
    };

    for (int i = 0; i < 4; i++)
    {
        // Rotate
        float rx = corners[i][0] * cosA - corners[i][1] * sinA;
        float ry = corners[i][0] * sinA + corners[i][1] * cosA;
        
        // World position
        float wx = cx + rx;
        float wy = cy + ry;

        // Apply Camera to world position
        SDL_FRect worldPoint = { wx, wy, 0, 0 };
        SDL_FRect screenPoint = ApplyCamera(worldPoint);

        v[i].position = { screenPoint.x, screenPoint.y };
        v[i].color = color;
    }

    // UVs
    v[0].tex_coord = { u0, v0 };
    v[1].tex_coord = { u1, v0 };
    v[2].tex_coord = { u1, v1 };
    v[3].tex_coord = { u0, v1 };

    batch.vertices.insert(batch.vertices.end(), &v[0], &v[4]);

    // indices
    batch.indices.push_back(start + 0);
    batch.indices.push_back(start + 1);
    batch.indices.push_back(start + 2);

    batch.indices.push_back(start + 0);
    batch.indices.push_back(start + 2);
    batch.indices.push_back(start + 3);
}
void SDLRenderer2D::AddColoredQuad(
    GeometryBatch& batch,
    const SDL_FRect& dst,
    SDL_FColor color)
{
    int start = (int)batch.vertices.size();

    SDL_Vertex v[4];

    v[0].position = { dst.x, dst.y };
    v[1].position = { dst.x + dst.w, dst.y };
    v[2].position = { dst.x + dst.w, dst.y + dst.h };
    v[3].position = { dst.x, dst.y + dst.h };

    // full white texture
    v[0].tex_coord = { 0,0 };
    v[1].tex_coord = { 1,0 };
    v[2].tex_coord = { 1,1 };
    v[3].tex_coord = { 0,1 };

    for (int i = 0; i < 4; i++)
        v[i].color = color;

    batch.vertices.insert(batch.vertices.end(), &v[0], &v[4]);

    batch.indices.push_back(start + 0);
    batch.indices.push_back(start + 1);
    batch.indices.push_back(start + 2);

    batch.indices.push_back(start + 0);
    batch.indices.push_back(start + 2);
    batch.indices.push_back(start + 3);
}

// -----------------------------------------------------------------------------
// Screen-space UI draw methods (no camera transform)
// -----------------------------------------------------------------------------

void SDLRenderer2D::DrawRectScreen(Components::Rect rect, Components::Color color, bool fill)
{
    SDL_FRect sdlRect = { rect.x, rect.y, rect.w, rect.h };
    SDL_Color sdlColor = {
        static_cast<Uint8>(color.r),
        static_cast<Uint8>(color.g),
        static_cast<Uint8>(color.b),
        static_cast<Uint8>(color.a)
    };
    m_uiRects.push_back({ sdlRect, sdlColor, fill });
}

void SDLRenderer2D::PushTextScreen(uint32_t textureId, float width, float height,
                                    float x, float y, Components::TextAlign /*align*/)
{
    auto it = m_textures.find(textureId);
    if (it == m_textures.end() || !it->second.ptr) return;

    m_uiTexts.push_back({ it->second.ptr, x, y, width, height });
}

void SDLRenderer2D::PushSpriteScreen(const Components::Texture& texture, Components::Rect destRect)
{
    auto it = m_textures.find(texture.id);
    if (it == m_textures.end() || !it->second.ptr) return;

    SDL_FRect dst = { destRect.x, destRect.y, destRect.w, destRect.h };
    m_uiSprites.push_back({ it->second.ptr, dst });
}