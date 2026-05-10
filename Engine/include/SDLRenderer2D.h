#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Components.h"
#include "IRenderer2D.h"
#include <SDL3_ttf/SDL_ttf.h>

struct GeometryBatch
{
    SDL_Texture* texture = nullptr;
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices;
};

struct SDLTextureEntry
{
    Components::Texture meta;
    SDL_Texture* ptr;
};

struct SDLRendererData
{
    SDL_Texture* texture;
    SDL_FRect srcRect;
    SDL_FRect dstRect;
    float angle;
    SDL_FPoint center;
    SDL_FlipMode flip;
};

struct PrimitiveData
{
    enum class Type { FilledRect, Rect, Line } type;
    SDL_FRect rect;
    float x1, y1, x2, y2;
    SDL_Color color;
};

class SDLRenderer2D: public IRenderer2D
{
public:
    SDL_Texture* CreateWhiteTexture();
    SDLRenderer2D(std::shared_ptr<SDL_Renderer> renderer);
    ~SDLRenderer2D();

    void BeginFrame() override;
    void CallRender() override;
    void EndFrame(int windowW, int windowH) override;
    void SetCamera(Components::CameraComponent* camera) override { m_Camera = camera; }
    Components::CameraBounds GetCameraBounds() const override {
        if (!m_Camera)
        {
            return { 0, 0, (float)m_virtualWidth, (float)m_virtualHeight };
        }

        float viewW = m_virtualWidth / m_Camera->zoom;
        float viewH = m_virtualHeight / m_Camera->zoom;

        return {
            m_Camera->position.x - viewW * 0.5f,
            m_Camera->position.y - viewH * 0.5f,
            viewW,
            viewH
        };
    }
    void PushSpriteToRenderQueue(const Components::Sprite& sprite, const Components::Transform2D& transform) override;
    void PushTextToRenderQueue(uint32_t textureId, float width, float height,
        const Components::Transform2D& transform, int layer, Components::TextAlign align) override;

    void DrawRect(Components::Rect rect, Components::Color color, bool fill, int layer) override;
    void DrawLine(float x1, float y1, float x2, float y2, Components::Color color, int layer) override;

    // Screen-space UI draw (no camera transform)
    void DrawRectScreen(Components::Rect rect, Components::Color color, bool fill) override;
    void PushTextScreen(uint32_t textureId, float width, float height,
                        float x, float y, Components::TextAlign align) override;
    void PushSpriteScreen(const Components::Texture& texture, Components::Rect destRect) override;
    Vec2 ScreenToWorld(Vec2 screenPos) override;

    Components::Texture GetTextureFromFile(const std::string& path) override;
    uint32_t LoadFont(const std::string& path, float size) override;
    void UnloadFont(uint32_t fontId) override;

    uint32_t RenderTextToTexture(uint32_t fontId, const std::string& text,
        Components::Color color, bool wordWrap, int wrapWidth,
        float& outWidth, float& outHeight) override;
    SDL_FRect ApplyCamera(const SDL_FRect& rect);
    void AddSpriteToBatch(GeometryBatch& batch, const SDL_FRect& src, const SDL_FRect& dst, const Components::Transform2D& transform, SDLTextureEntry* textureEntry, SDL_FColor color);

    void AddColoredQuad(GeometryBatch& batch, const SDL_FRect& dst, SDL_FColor color);

private:
   

    std::shared_ptr<SDL_Renderer> m_renderer;

    std::unordered_map<uint32_t, SDLTextureEntry> m_textures;
    std::unordered_map<uint32_t, TTF_Font*> m_fonts;

    std::unordered_map<int, std::unordered_map<SDL_Texture*, GeometryBatch>> m_Batches;



    uint32_t m_nextId = 1;
    uint32_t m_nextFontId = 1;
    Components::CameraComponent* m_Camera = nullptr;
    SDL_Texture* m_whiteTexture = nullptr;
    SDLTextureEntry m_whiteTextureEntry = {
    { 0, {1.0f, 1.0f} },
    m_whiteTexture
    };
    int m_virtualWidth = 320;
    int m_virtualHeight = 180;

    // ---- UI screen-space queues (flushed after world render in EndFrame) ----
    struct UIRectEntry   { SDL_FRect rect; SDL_Color color; bool fill; };
    struct UITextEntry   { SDL_Texture* tex; float x, y, w, h; };
    struct UISpriteEntry { SDL_Texture* tex; SDL_FRect dst; };

    std::vector<UIRectEntry>   m_uiRects;
    std::vector<UITextEntry>   m_uiTexts;
    std::vector<UISpriteEntry> m_uiSprites;
    GeometryBatch              m_uiWhiteBatch; // batched rects, flushed each EndFrame
};