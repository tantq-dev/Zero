#include "ResourcesManager.h"
#include "IRenderer2D.h"
#include "AudioSystem.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

// ============================================================================
// Helpers
// ============================================================================
std::string ResourcesManager::FontKey(std::string_view path, float size)
{
    // e.g. "assets/fonts/main.ttf|24.0"
    return std::string(path) + "|" + std::to_string(size);
}

// ============================================================================
// TEXTURES
// ============================================================================
uint32_t ResourcesManager::GetOrLoadTexture(std::string_view path, IRenderer2D& renderer)
{
    const std::string key(path);

    // Dedup — already loaded?
    auto it = m_textureByPath.find(key);
    if (it != m_textureByPath.end())
        return it->second;

    // Ask renderer to load (uses stb_image internally)
    Components::Texture tex = renderer.GetTextureFromFile(key);
    if (tex.id == 0)
    {
        LOG_ERROR("ResourcesManager: failed to load texture: " + key);
        return 0;
    }

    m_textureById[tex.id]  = tex;
    m_textureByPath[key]   = tex.id;
    return tex.id;
}

uint32_t ResourcesManager::StoreTexture(std::string_view path, Components::Texture texture)
{
    const std::string key(path);
    m_textureById[texture.id]  = texture;
    m_textureByPath[key]       = texture.id;
    return texture.id;
}

const Components::Texture* ResourcesManager::GetTexture(uint32_t id) const
{
    auto it = m_textureById.find(id);
    return it != m_textureById.end() ? &it->second : nullptr;
}

const Components::Texture* ResourcesManager::GetTexture(std::string_view path) const
{
    auto it = m_textureByPath.find(std::string(path));
    if (it == m_textureByPath.end()) return nullptr;
    return GetTexture(it->second);
}

void ResourcesManager::RemoveTexture(uint32_t id)
{
    auto it = m_textureById.find(id);
    if (it == m_textureById.end()) return;

    // Remove reverse-lookup entry
    for (auto pit = m_textureByPath.begin(); pit != m_textureByPath.end(); )
    {
        if (pit->second == id)
            pit = m_textureByPath.erase(pit);
        else
            ++pit;
    }

    m_textureById.erase(it);
    // Note: the SDL_Texture* is owned by SDLRenderer2D — we only remove metadata.
}

void ResourcesManager::ClearTextures()
{
    m_textureById.clear();
    m_textureByPath.clear();
}

// ============================================================================
// SPRITE SHEETS
// ============================================================================
void ResourcesManager::StoreSpriteSheet(uint32_t id, Components::SpriteSheet sheet)
{
    m_spriteSheets.insert_or_assign(id, std::move(sheet));
}

Components::SpriteSheet* ResourcesManager::GetSpriteSheet(uint32_t id)
{
    auto it = m_spriteSheets.find(id);
    return it != m_spriteSheets.end() ? &it->second : nullptr;
}

const Components::SpriteSheet* ResourcesManager::GetSpriteSheet(uint32_t id) const
{
    auto it = m_spriteSheets.find(id);
    return it != m_spriteSheets.end() ? &it->second : nullptr;
}

void ResourcesManager::RemoveSpriteSheet(uint32_t id)
{
    m_spriteSheets.erase(id);
}

void ResourcesManager::ClearSpriteSheets()
{
    m_spriteSheets.clear();
}

// ============================================================================
// AUDIO
// ============================================================================
uint32_t ResourcesManager::GetOrLoadAudio(std::string_view path, System::AudioSystem& audio)
{
    const std::string key(path);

    // Dedup
    auto it = m_audioByPath.find(key);
    if (it != m_audioByPath.end())
        return it->second;

    // Ask AudioSystem to load (auto-detects .wav / .mp3 by extension)
    uint32_t clipId = audio.LoadAudio(key);
    if (clipId == 0)
    {
        LOG_ERROR("ResourcesManager: failed to load audio: " + key);
        return 0;
    }

    m_audioByPath[key] = clipId;
    return clipId;
}

void ResourcesManager::StoreAudioClip(std::string_view path, uint32_t clipId)
{
    m_audioByPath[std::string(path)] = clipId;
}

uint32_t ResourcesManager::GetAudioClipId(std::string_view path) const
{
    auto it = m_audioByPath.find(std::string(path));
    return it != m_audioByPath.end() ? it->second : 0;
}

void ResourcesManager::RemoveAudioClip(std::string_view path, System::AudioSystem& audio)
{
    const std::string key(path);
    auto it = m_audioByPath.find(key);
    if (it == m_audioByPath.end()) return;

    audio.UnloadClip(it->second);
    m_audioByPath.erase(it);
}

void ResourcesManager::ClearAudioClips(System::AudioSystem& audio)
{
    for (auto& [path, clipId] : m_audioByPath)
        audio.UnloadClip(clipId);
    m_audioByPath.clear();
}

// ============================================================================
// FONTS
// ============================================================================
uint32_t ResourcesManager::GetOrLoadFont(std::string_view path, float size, IRenderer2D& renderer)
{
    const std::string key = FontKey(path, size);

    // Dedup
    auto it = m_fontByKey.find(key);
    if (it != m_fontByKey.end())
        return it->second;

    uint32_t fontId = renderer.LoadFont(std::string(path), size);
    if (fontId == 0)
    {
        LOG_ERROR("ResourcesManager: failed to load font: " + std::string(path));
        return 0;
    }

    m_fontByKey[key] = fontId;
    return fontId;
}

uint32_t ResourcesManager::GetFontId(std::string_view path, float size) const
{
    auto it = m_fontByKey.find(FontKey(path, size));
    return it != m_fontByKey.end() ? it->second : 0;
}

bool ResourcesManager::HasFont(std::string_view path, float size) const
{
    return m_fontByKey.count(FontKey(path, size)) > 0;
}

void ResourcesManager::RemoveFont(std::string_view path, float size, IRenderer2D& renderer)
{
    const std::string key = FontKey(path, size);
    auto it = m_fontByKey.find(key);
    if (it == m_fontByKey.end()) return;

    renderer.UnloadFont(it->second);
    m_fontByKey.erase(it);
}

void ResourcesManager::ClearFonts(IRenderer2D& renderer)
{
    for (auto& [key, fontId] : m_fontByKey)
        renderer.UnloadFont(fontId);
    m_fontByKey.clear();
}

// ============================================================================
// TEXT / JSON
// ============================================================================
const std::string* ResourcesManager::GetOrLoadText(std::string_view path)
{
    const std::string key(path);

    // Dedup
    auto it = m_textByPath.find(key);
    if (it != m_textByPath.end())
        return &it->second;

    // Read whole file into string
    std::ifstream file(key, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR("ResourcesManager: failed to open text file: " + key);
        return nullptr;
    }

    std::ostringstream ss;
    ss << file.rdbuf();

    m_textByPath[key] = ss.str();
    return &m_textByPath[key];
}

void ResourcesManager::StoreText(std::string_view path, std::string content)
{
    m_textByPath.insert_or_assign(std::string(path), std::move(content));
}

const std::string* ResourcesManager::GetText(std::string_view path) const
{
    auto it = m_textByPath.find(std::string(path));
    return it != m_textByPath.end() ? &it->second : nullptr;
}

void ResourcesManager::RemoveText(std::string_view path)
{
    m_textByPath.erase(std::string(path));
}

void ResourcesManager::ClearTexts()
{
    m_textByPath.clear();
}

// ============================================================================
// ClearAll
// ============================================================================
void ResourcesManager::ClearAll(IRenderer2D& renderer, System::AudioSystem& audio)
{
    ClearTextures();
    ClearSpriteSheets();
    ClearAudioClips(audio);
    ClearFonts(renderer);
    ClearTexts();
}
