#include "AudioSystem.h"
#include "Logger.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include "dr_mp3.h"
#include "dr_wav.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>

namespace System
{
	AudioSystem::~AudioSystem()
	{
		Shutdown();
	}

	bool AudioSystem::Initialize()
	{
		if (m_initialized) return true;

		// SDL_INIT_AUDIO should already be called in Game::Game(); guard just in case.
		if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
		{
			if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
			{
				LOG_ERROR("AudioSystem: Failed to init SDL audio subsystem: " + std::string(SDL_GetError()));
				return false;
			}
		}

		m_deviceSpec.freq     = 44100;
		m_deviceSpec.format   = SDL_AUDIO_S16;
		m_deviceSpec.channels = 2;

		m_deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_deviceSpec);
		if (m_deviceId == 0)
		{
			LOG_ERROR("AudioSystem: Failed to open audio device: " + std::string(SDL_GetError()));
			return false;
		}

		m_initialized = true;
		LOG_INFO("AudioSystem: Initialized (device=" + std::to_string(m_deviceId)
			+ ", freq=" + std::to_string(m_deviceSpec.freq)
			+ ", channels=" + std::to_string(m_deviceSpec.channels) + ")");

		return true;
	}

	void AudioSystem::Shutdown()
	{
		if (!m_initialized) return;

		for (auto& [id, clip] : m_clips)
		{
			if (clip.buffer)
			{
				SDL_free(clip.buffer);
				clip.buffer = nullptr;
			}
		}
		m_clips.clear();

		if (m_deviceId != 0)
		{
			SDL_CloseAudioDevice(m_deviceId);
			m_deviceId = 0;
		}

		m_initialized = false;
		LOG_INFO("AudioSystem: Shutdown complete.");
	}

	// ---------------------------------------------------------------------------
	// LoadAudio — unified, auto-detects .wav / .mp3 by extension
	// ---------------------------------------------------------------------------
	uint32_t AudioSystem::LoadAudio(const std::string& path)
	{
		if (!m_initialized)
		{
			LOG_ERROR("AudioSystem: Cannot load audio — system not initialized.");
			return 0;
		}

		// Extract lowercase extension
		std::string ext;
		size_t dotPos = path.rfind('.');
		if (dotPos != std::string::npos)
		{
			ext = path.substr(dotPos + 1);
			std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return (char)std::tolower(c); });
		}

		if (ext == "mp3")
			return LoadMP3(path);
		else
			return LoadWAV(path);   // handles .wav (and falls back for anything unknown)
	}

	// ---------------------------------------------------------------------------
	// Helper: read a file into memory using SDL IO (works in Emscripten VFS)
	// ---------------------------------------------------------------------------
	static std::vector<uint8_t> ReadFileBytes(const std::string& path)
	{
		SDL_IOStream* io = SDL_IOFromFile(path.c_str(), "rb");
		if (!io)
		{
			LOG_ERROR("AudioSystem: Cannot open file '" + path + "': " + std::string(SDL_GetError()));
			return {};
		}
		Sint64 size = SDL_GetIOSize(io);
		if (size <= 0)
		{
			SDL_CloseIO(io);
			return {};
		}
		std::vector<uint8_t> buf(static_cast<size_t>(size));
		SDL_ReadIO(io, buf.data(), buf.size());
		SDL_CloseIO(io);
		return buf;
	}

	// ---------------------------------------------------------------------------
	// LoadWAV — decoded by dr_wav from memory (Emscripten-safe via SDL IO)
	// ---------------------------------------------------------------------------
	uint32_t AudioSystem::LoadWAV(const std::string& path)
	{
		if (!m_initialized)
		{
			LOG_ERROR("AudioSystem: Cannot load WAV — system not initialized.");
			return 0;
		}

		auto fileData = ReadFileBytes(path);
		if (fileData.empty())
		{
			LOG_ERROR("AudioSystem: Failed to read file '" + path + "'");
			return 0;
		}

		drwav_uint32 channels   = 0;
		drwav_uint32 sampleRate = 0;
		drwav_uint64 frameCount = 0;
		drwav_int16* pSamples   = drwav_open_memory_and_read_pcm_frames_s16(
			fileData.data(), fileData.size(),
			&channels, &sampleRate, &frameCount, nullptr);

		if (!pSamples)
		{
			LOG_ERROR("AudioSystem: dr_wav failed to decode '" + path + "'");
			return 0;
		}

		size_t byteCount = static_cast<size_t>(frameCount) * channels * sizeof(drwav_int16);
		void*  copy      = SDL_malloc(byteCount);
		std::memcpy(copy, pSamples, byteCount);
		drwav_free(pSamples, nullptr);

		SDL_AudioSpec spec{};
		spec.freq     = static_cast<int>(sampleRate);
		spec.format   = SDL_AUDIO_S16;
		spec.channels = static_cast<int>(channels);

		uint32_t clipId = m_nextClipId++;
		Components::AudioClip clip;
		clip.spec     = spec;
		clip.buffer   = static_cast<uint8_t*>(copy);
		clip.length   = static_cast<uint32_t>(byteCount);
		clip.filePath = path;
		m_clips[clipId] = clip;

		LOG_INFO("AudioSystem: Loaded WAV '" + path + "' as clipId=" + std::to_string(clipId)
			+ " (freq=" + std::to_string(sampleRate)
			+ ", ch=" + std::to_string(channels)
			+ ", bytes=" + std::to_string(byteCount) + ")");

		return clipId;
	}

	// ---------------------------------------------------------------------------
	// LoadMP3 — decoded by dr_mp3 from memory (Emscripten-safe via SDL IO)
	// ---------------------------------------------------------------------------
	uint32_t AudioSystem::LoadMP3(const std::string& path)
	{
		if (!m_initialized)
		{
			LOG_ERROR("AudioSystem: Cannot load MP3 — system not initialized.");
			return 0;
		}

		auto fileData = ReadFileBytes(path);
		if (fileData.empty())
		{
			LOG_ERROR("AudioSystem: Failed to read file '" + path + "'");
			return 0;
		}

		drmp3_config   config{};
		drmp3_uint64   frameCount = 0;
		drmp3_int16*   pSamples   = drmp3_open_memory_and_read_pcm_frames_s16(
			fileData.data(), fileData.size(),
			&config, &frameCount, nullptr);

		if (!pSamples)
		{
			LOG_ERROR("AudioSystem: dr_mp3 failed to decode '" + path + "'");
			return 0;
		}

		size_t byteCount = static_cast<size_t>(frameCount) * config.channels * sizeof(drmp3_int16);
		void*  copy      = SDL_malloc(byteCount);
		std::memcpy(copy, pSamples, byteCount);
		drmp3_free(pSamples, nullptr);

		SDL_AudioSpec spec{};
		spec.freq     = static_cast<int>(config.sampleRate);
		spec.format   = SDL_AUDIO_S16;
		spec.channels = static_cast<int>(config.channels);

		uint32_t clipId = m_nextClipId++;
		Components::AudioClip clip;
		clip.spec     = spec;
		clip.buffer   = static_cast<uint8_t*>(copy);
		clip.length   = static_cast<uint32_t>(byteCount);
		clip.filePath = path;
		m_clips[clipId] = clip;

		LOG_INFO("AudioSystem: Loaded MP3 '" + path + "' as clipId=" + std::to_string(clipId)
			+ " (freq=" + std::to_string(config.sampleRate)
			+ ", ch=" + std::to_string(config.channels)
			+ ", bytes=" + std::to_string(byteCount) + ")");

		return clipId;
	}

	void AudioSystem::UnloadClip(uint32_t clipId)
	{
		auto it = m_clips.find(clipId);
		if (it == m_clips.end()) return;

		if (it->second.buffer)
		{
			SDL_free(it->second.buffer);
		}
		m_clips.erase(it);
		LOG_INFO("AudioSystem: Unloaded clipId=" + std::to_string(clipId));
	}

	void AudioSystem::SetMasterVolume(float volume)
	{
		m_masterVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
	}

	void AudioSystem::Update(entt::registry& registry)
	{
		if (!m_initialized) return;

		auto view = registry.view<Components::AudioSource>();
		for (auto entity : view)
		{
			auto& src = view.get<Components::AudioSource>(entity);

			// Handle playOnAwake (first time only)
			if (src.playOnAwake && !src._awakened)
			{
				src._awakened  = true;
				src.requestPlay = true;
			}

			// Handle stop request
			if (src.requestStop)
			{
				src.requestStop = false;
				StopSource(src);
			}

			// Handle play request
			if (src.requestPlay)
			{
				src.requestPlay = false;
				PlaySource(src);
			}

			// If looping and stream has drained, re-feed it
			if (src.isPlaying && src.loop && src._stream)
			{
				int queued = SDL_GetAudioStreamQueued(src._stream);
				if (queued <= 0)
				{
					auto clipIt = m_clips.find(src.clipId);
					if (clipIt != m_clips.end())
					{
						SDL_PutAudioStreamData(src._stream, clipIt->second.buffer, clipIt->second.length);
					}
				}
			}

			// If NOT looping and stream has drained, mark as stopped
			if (src.isPlaying && !src.loop && src._stream)
			{
				int queued = SDL_GetAudioStreamQueued(src._stream);
				if (queued <= 0)
				{
					src.isPlaying = false;
					SDL_DestroyAudioStream(src._stream);
					src._stream = nullptr;
				}
			}
		}
	}

	void AudioSystem::PlaySource(Components::AudioSource& src)
	{
		if (src.clipId == 0) return;

		auto clipIt = m_clips.find(src.clipId);
		if (clipIt == m_clips.end())
		{
			LOG_ERROR("AudioSystem: clipId=" + std::to_string(src.clipId) + " not found.");
			return;
		}

		const auto& clip = clipIt->second;

		// If already playing, stop first (restart behaviour)
		if (src._stream)
		{
			SDL_DestroyAudioStream(src._stream);
			src._stream = nullptr;
		}

		// Create a new audio stream: source format → device format
		src._stream = SDL_CreateAudioStream(&clip.spec, &m_deviceSpec);
		if (!src._stream)
		{
			LOG_ERROR("AudioSystem: Failed to create audio stream: " + std::string(SDL_GetError()));
			src.isPlaying = false;
			return;
		}

		// Volume = source × master
		SDL_SetAudioStreamGain(src._stream, src.volume * m_masterVolume);

		// Feed the decoded PCM data
		if (!SDL_PutAudioStreamData(src._stream, clip.buffer, clip.length))
		{
			LOG_ERROR("AudioSystem: Failed to put audio data: " + std::string(SDL_GetError()));
			SDL_DestroyAudioStream(src._stream);
			src._stream   = nullptr;
			src.isPlaying = false;
			return;
		}

		// Bind stream to the output device so SDL mixes it automatically
		if (!SDL_BindAudioStream(m_deviceId, src._stream))
		{
			LOG_ERROR("AudioSystem: Failed to bind stream to device: " + std::string(SDL_GetError()));
			SDL_DestroyAudioStream(src._stream);
			src._stream   = nullptr;
			src.isPlaying = false;
			return;
		}

		src.isPlaying = true;
	}

	void AudioSystem::StopSource(Components::AudioSource& src)
	{
		if (src._stream)
		{
			SDL_DestroyAudioStream(src._stream);
			src._stream = nullptr;
		}
		src.isPlaying = false;
	}

	void AudioSystem::StopAll(entt::registry& registry)
	{
		auto view = registry.view<Components::AudioSource>();
		for (auto entity : view)
		{
			auto& src = view.get<Components::AudioSource>(entity);
			StopSource(src);
		}
	}
}
