#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <string_view>
#include "Components.h"

// Forward declarations — avoids pulling in heavy SDL/entt headers here
class IRenderer2D;
namespace System { class AudioSystem; }

// ============================================================================
//  ResourcesManager
//
//  Central cache for all game assets. Integrates with the engine's existing
//  systems (SDLRenderer2D for textures/fonts, AudioSystem for audio clips).
//
//  Design principles:
//   - Path-based deduplication: loading the same file twice returns the same ID
//   - Lazy loading: assets loaded on first request, never duplicated
//   - Move-aware storage: all store operations use std::move
//   - Crash-safe getters: return nullptr / 0 instead of throwing
//   - Explicit lifetime: Clear() per-type or ClearAll() on scene unload
//   - Non-copyable, owned by Game (via unique_ptr)
// ============================================================================
class ResourcesManager
{
public:
    ResourcesManager()  = default;
    ~ResourcesManager() = default;

    // Non-copyable, non-movable (owned by Game as unique_ptr)
    ResourcesManager(const ResourcesManager&)            = delete;
    ResourcesManager& operator=(const ResourcesManager&) = delete;
    ResourcesManager(ResourcesManager&&)                 = delete;
    ResourcesManager& operator=(ResourcesManager&&)      = delete;

    // =========================================================================
    // TEXTURES  (PNG / JPG — any format stb_image supports)
    //   The renderer owns the SDL_Texture*. ResourcesManager caches the
    //   Components::Texture handle (id + size) and deduplicates by path.
    // =========================================================================

    /// Returns 0 on failure.
    /// If the path was already loaded, returns the existing ID (no re-load).
    uint32_t GetOrLoadTexture(std::string_view path, IRenderer2D& renderer);

    /// Direct store — use when you already have a handle (e.g. procedural texture).
    uint32_t StoreTexture(std::string_view path, Components::Texture texture);

    [[nodiscard]] const Components::Texture* GetTexture(uint32_t id) const;
    [[nodiscard]] const Components::Texture* GetTexture(std::string_view path) const;

    bool HasTexture(uint32_t id)        const { return m_textureById.count(id) > 0; }
    bool HasTexture(std::string_view p) const { return m_textureByPath.count(std::string(p)) > 0; }

    void RemoveTexture(uint32_t id);
    void ClearTextures();

    // =========================================================================
    // SPRITE SHEETS
    //   Metadata (texture handle + frame rects). Texture must already be loaded.
    // =========================================================================

    void StoreSpriteSheet(uint32_t id, Components::SpriteSheet sheet);

    [[nodiscard]] Components::SpriteSheet*       GetSpriteSheet(uint32_t id);
    [[nodiscard]] const Components::SpriteSheet* GetSpriteSheet(uint32_t id) const;

    bool HasSpriteSheet(uint32_t id) const { return m_spriteSheets.count(id) > 0; }

    void RemoveSpriteSheet(uint32_t id);
    void ClearSpriteSheets();

    // =========================================================================
    // SPRITE ATLASES
    //   Rich metadata with named animations. Loads from JSON.
    // =========================================================================

    /// Returns 0 on failure. Texture path and JSON path are required.
    uint32_t GetOrLoadSpriteAtlas(std::string_view jsonPath, std::string_view texturePath, IRenderer2D& renderer);

    void StoreSpriteAtlas(uint32_t id, Components::SpriteAtlas atlas);

    [[nodiscard]] Components::SpriteAtlas*       GetSpriteAtlas(uint32_t id);
    [[nodiscard]] const Components::SpriteAtlas* GetSpriteAtlas(uint32_t id) const;

    bool HasSpriteAtlas(uint32_t id) const { return m_spriteAtlases.count(id) > 0; }

    void RemoveSpriteAtlas(uint32_t id);
    void ClearSpriteAtlases();

    // =========================================================================
    // AUDIO CLIPS  (.wav / .mp3 — auto-detected from extension)
    //   AudioSystem owns the decoded buffer. ResourcesManager caches the clip ID
    //   and deduplicates by path so callers never double-load.
    // =========================================================================

    /// Returns 0 on failure. Returns existing ID if already loaded.
    uint32_t GetOrLoadAudio(std::string_view path, System::AudioSystem& audio);

    /// Direct store for a clip ID already returned by AudioSystem.
    void StoreAudioClip(std::string_view path, uint32_t clipId);

    [[nodiscard]] uint32_t GetAudioClipId(std::string_view path) const;
    bool HasAudioClip(std::string_view path) const { return m_audioByPath.count(std::string(path)) > 0; }

    void RemoveAudioClip(std::string_view path, System::AudioSystem& audio);
    void ClearAudioClips(System::AudioSystem& audio);

    // =========================================================================
    // FONTS  (.ttf — via SDL3_ttf)
    //   SDLRenderer2D owns the TTF_Font*. ResourcesManager deduplicates by
    //   (path, size) pair — same .ttf at different sizes = separate entries.
    // =========================================================================

    /// Returns 0 on failure. Returns existing ID if already loaded.
    uint32_t GetOrLoadFont(std::string_view path, float size, IRenderer2D& renderer);

    [[nodiscard]] uint32_t GetFontId(std::string_view path, float size) const;
    bool HasFont(std::string_view path, float size) const;

    void RemoveFont(std::string_view path, float size, IRenderer2D& renderer);
    void ClearFonts(IRenderer2D& renderer);

    // =========================================================================
    // TEXT / JSON  (raw string content — any text file loaded into memory)
    // =========================================================================

    /// Reads entire file into a std::string and caches it. Returns nullptr on failure.
    /// Returns cached string if already loaded.
    const std::string* GetOrLoadText(std::string_view path);

    /// Direct store — e.g. for in-memory / procedural text assets.
    void StoreText(std::string_view path, std::string content);

    [[nodiscard]] const std::string* GetText(std::string_view path) const;
    bool HasText(std::string_view path) const { return m_textByPath.count(std::string(path)) > 0; }

    void RemoveText(std::string_view path);
    void ClearTexts();

    // =========================================================================
    // Lifetime — call on scene unload
    // =========================================================================

    /// Clears ALL cached metadata and unloads GPU/audio handles via their systems.
    void ClearAll(IRenderer2D& renderer, System::AudioSystem& audio);

private:
    // ---- Texture cache ----
    std::unordered_map<uint32_t,    Components::Texture>  m_textureById;
    std::unordered_map<std::string, uint32_t>             m_textureByPath; // path → id

    // ---- SpriteSheet cache ----
    std::unordered_map<uint32_t, Components::SpriteSheet> m_spriteSheets;

    // ---- SpriteAtlas cache ----
    std::unordered_map<uint32_t, Components::SpriteAtlas> m_spriteAtlases;
    std::unordered_map<std::string, uint32_t>             m_atlasByPath; // jsonPath → id

    // ---- Audio cache ----
    std::unordered_map<std::string, uint32_t> m_audioByPath; // path → clipId

    // ---- Font cache ----
    // Key = "path|size" (e.g. "assets/fonts/main.ttf|24.000000")
    std::unordered_map<std::string, uint32_t> m_fontByKey;   // key → fontId

    // ---- Text/JSON cache ----
    std::unordered_map<std::string, std::string> m_textByPath;

    // ---- Internal helpers ----
    static std::string FontKey(std::string_view path, float size);
};
