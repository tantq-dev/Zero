#pragma once
#include "SDL3/SDL.h"
#include "IRenderer2D.h"
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>

struct SDLTextureEntry 
{ 
    Components::Texture texture; 
    SDL_Texture* ptr = nullptr;
};

struct SDLRendererData 
{
    SDL_Texture* texture;        
    SDL_FRect srcRect;          
    SDL_FRect dstRect;          
    double angle;
    SDL_FPoint center;           
    SDL_FlipMode flip;

    SDLRendererData() = default;
    
    SDLRendererData(SDL_Texture* tex,
                    SDL_FRect src,
                    SDL_FRect dst,
                    double ang,
                    SDL_FPoint ctr,
                    SDL_FlipMode flp = SDL_FLIP_NONE)
        : texture(tex)
        , srcRect(src)
        , dstRect(dst)
        , angle(ang)
        , center(ctr)
        , flip(flp)
    {}
};

class SDLRenderer : public IRenderer2D 
{
public:
    explicit SDLRenderer(std::shared_ptr<SDL_Renderer> renderer);
    ~SDLRenderer() override;
    
    // Delete copy operations (we manage raw SDL_Texture pointers)
    SDLRenderer(const SDLRenderer&) = delete;
    SDLRenderer& operator=(const SDLRenderer&) = delete;
    
    void PushSpriteToRenderQueue(const Components::Sprite& sprite, 
                                  const Components::Transform2D& transform) override;
    Components::Texture GetTextureFromFile(const std::string& path) override;
    void CallRender() override;

private:
    std::shared_ptr<SDL_Renderer> m_renderer;
    uint32_t m_nextId = 0;
    std::unordered_map<uint32_t, SDLTextureEntry> m_textures;
    std::map<uint32_t, std::vector<SDLRendererData>> m_RenderQueue;
};