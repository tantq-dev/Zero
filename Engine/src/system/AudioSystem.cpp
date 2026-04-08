#include "AudioSystem.h"
#include "Logger.h"
#include <SDL3/SDL.h>

namespace System
{
	AudioSystem::~AudioSystem()
	{
		Shutdown();
	}

	bool AudioSystem::Initialize()
	{
		if (m_initialized) return true;

		// SDL_INIT_AUDIO should already be called; if not, init it now.
		if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
		{
			if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
			{
				LOG_ERROR("AudioSystem: Failed to init SDL audio subsystem: " + std::string(SDL_GetError()));
				return false;
			}
		}

		// Open the default playback device
		// SDL3: SDL_OpenAudioDevice with SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK
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

		// Free all clips
		for (auto& [id, clip] : m_clips)
		{
			if (clip.buffer)
			{
				SDL_free(clip.buffer);
				clip.buffer = nullptr;
			}
		}
		m_clips.clear();

		// Close device (this also destroys any bound streams)
		if (m_deviceId != 0)
		{
			SDL_CloseAudioDevice(m_deviceId);
			m_deviceId = 0;
		}

		m_initialized = false;
		LOG_INFO("AudioSystem: Shutdown complete.");
	}

	uint32_t AudioSystem::LoadWAV(const std::string& path)
	{
		if (!m_initialized)
		{
			LOG_ERROR("AudioSystem: Cannot load WAV — system not initialized.");
			return 0;
		}

		SDL_AudioSpec wavSpec{};
		uint8_t* wavBuffer = nullptr;
		uint32_t wavLength = 0;

		if (!SDL_LoadWAV(path.c_str(), &wavSpec, &wavBuffer, &wavLength))
		{
			LOG_ERROR("AudioSystem: Failed to load WAV '" + path + "': " + std::string(SDL_GetError()));
			return 0;
		}

		uint32_t clipId = m_nextClipId++;
		Components::AudioClip clip;
		clip.spec     = wavSpec;
		clip.buffer   = wavBuffer;
		clip.length   = wavLength;
		clip.filePath = path;

		m_clips[clipId] = clip;

		LOG_INFO("AudioSystem: Loaded WAV '" + path + "' as clipId=" + std::to_string(clipId)
			+ " (freq=" + std::to_string(wavSpec.freq)
			+ ", ch=" + std::to_string(wavSpec.channels)
			+ ", bytes=" + std::to_string(wavLength) + ")");

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
				src._awakened = true;
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

			// If looping and the stream has drained, re-feed it
			if (src.isPlaying && src.loop && src._stream)
			{
				int queued = SDL_GetAudioStreamQueued(src._stream);
				if (queued <= 0)
				{
					// Re-feed the buffer to loop
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

		// If already playing, stop first (restart behavior)
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

		// Set volume: combine source volume with master volume
		float effectiveVolume = src.volume * m_masterVolume;
		SDL_SetAudioStreamGain(src._stream, effectiveVolume);

		// Feed the audio data
		if (!SDL_PutAudioStreamData(src._stream, clip.buffer, clip.length))
		{
			LOG_ERROR("AudioSystem: Failed to put audio data: " + std::string(SDL_GetError()));
			SDL_DestroyAudioStream(src._stream);
			src._stream = nullptr;
			src.isPlaying = false;
			return;
		}

		// Bind stream to the output device so SDL mixes it automatically
		if (!SDL_BindAudioStream(m_deviceId, src._stream))
		{
			LOG_ERROR("AudioSystem: Failed to bind stream to device: " + std::string(SDL_GetError()));
			SDL_DestroyAudioStream(src._stream);
			src._stream = nullptr;
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
