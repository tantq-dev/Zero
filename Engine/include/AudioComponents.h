#pragma once
#include <cstdint>
#include <string>
#include <SDL3/SDL_audio.h>

namespace Components
{
	// Stored in AudioSystem's internal clip cache — not an ECS component
	struct AudioClip
	{
		SDL_AudioSpec spec{};
		uint8_t* buffer = nullptr;
		uint32_t  length = 0;       // buffer size in bytes
		std::string filePath;       // original file path (for debug)
	};

	// ECS component — attach to any entity to give it a sound
	struct AudioSource
	{
		uint32_t clipId       = 0;       // ID returned by AudioSystem::LoadAudio/LoadWAV/LoadMP3
		float    volume       = 1.0f;    // 0.0 – 1.0
		bool     loop         = false;
		bool     playOnAwake  = false;   // auto-play first time the system sees it
		bool     isPlaying    = false;   // managed by AudioSystem
		bool     requestPlay  = false;   // set true to start/restart playback
		bool     requestStop  = false;   // set true to stop playback

		// Internal — managed by AudioSystem. Do NOT set manually.
		SDL_AudioStream* _stream   = nullptr;
		bool             _awakened = false;  // has playOnAwake been processed?
	};
}
