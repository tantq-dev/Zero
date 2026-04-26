#pragma once
#include "entt.hpp"
#include "AudioComponents.h"
#include <string>
#include <unordered_map>
#include <SDL3/SDL_audio.h>

namespace System
{
	class AudioSystem
	{
	public:
		AudioSystem() = default;
		~AudioSystem();

		/// Open the SDL audio device. Call once at engine init.
		bool Initialize();

		/// Close device and free all loaded clips/streams.
		void Shutdown();

		/// Process every entity that has an AudioSource component.
		void Update(entt::registry& registry);

		// ---- Resource management ----

		/// Load an audio file, auto-detecting format from extension (.wav or .mp3).
		/// Returns a clipId (>0) on success, 0 on failure.
		uint32_t LoadAudio(const std::string& path);

		/// Load a WAV file (decoded via dr_wav, SDL IO-safe for Emscripten).
		uint32_t LoadWAV(const std::string& path);

		/// Load an MP3 file (decoded via dr_mp3, SDL IO-safe for Emscripten).
		uint32_t LoadMP3(const std::string& path);

		/// Free a previously loaded clip.
		void UnloadClip(uint32_t clipId);

		// ---- Global controls ----

		void  SetMasterVolume(float volume);
		float GetMasterVolume() const { return m_masterVolume; }

		/// Stop all currently playing audio sources.
		void StopAll(entt::registry& registry);

	private:
		void PlaySource(Components::AudioSource& src);
		void StopSource(Components::AudioSource& src);

		SDL_AudioDeviceID m_deviceId = 0;
		SDL_AudioSpec     m_deviceSpec{};
		bool              m_initialized = false;

		float m_masterVolume = 1.0f;

		// Clip storage  (clipId → AudioClip)
		uint32_t m_nextClipId = 1;
		std::unordered_map<uint32_t, Components::AudioClip> m_clips;

		static constexpr int MAX_CONCURRENT_SOUNDS = 32;
	};
}
