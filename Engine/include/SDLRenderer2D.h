#pragma once
#include "SDL3/SDL.h"
#include "IRenderer2D.h"
#include <memory>
#include <map>

struct SDLTextureEntry { Components::Texture texture; SDL_Texture* ptr = nullptr;};
struct SDLRendererData {
	SDL_Texture* texture;
		const SDL_FRect* srcrect = nullptr;
	const SDL_FRect* dstrect = nullptr;
	const double angle;
	const SDL_FPoint* center = nullptr;
	const SDL_FlipMode flip;
	SDLRendererData() = default;
	SDLRendererData(SDL_Texture* tex = nullptr,
	const SDL_FRect* src = nullptr,
	const SDL_FRect* dst = nullptr,
	const double angle = 0,
	const SDL_FPoint* center = nullptr,
	const SDL_FlipMode flip = SDL_FLIP_NONE) : texture(tex), srcrect(src), dstrect(dst), angle(angle), center(center), flip(flip) {}


};
class SDLRenderer : public IRenderer2D {
public:
	explicit SDLRenderer(std::shared_ptr<SDL_Renderer> renderer);
	~SDLRenderer() override;
	SDLRenderer() = delete;
	void PushSpriteToRenderQueue(const Components::Sprite& sprite, const Components::Transform2D& transform) override;
	Components::Texture GetTextureFromFile(const std::string& path) override;
	void CallRender() override;

private:
	
	std::shared_ptr<SDL_Renderer> m_renderer = nullptr;
	uint32_t m_nextId = 0;
	std::unordered_map<uint32_t, SDLTextureEntry> m_textures;
	std::map<uint32_t, std::vector<SDLRendererData>> m_RenderQueue;
};