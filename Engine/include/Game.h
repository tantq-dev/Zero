#pragma once  
#include <memory>  
#include <string>  
#include <unordered_map>  
#include "Window.h"  
#include "Scene.h"  
#ifdef ZERO_USE_IMGUI
#include "imgui.h"
#endif
#include "RenderSystem.h"
#include "InputSystem.h"
#include <AnimationSystem.h>
#include "AudioSystem.h"
#include "ActorComponentSystem.h"
#include <cstdint>
#include "ResourcesManager.h"

#define DEFAULT_FIXED_UPDATE_FRAME 60

namespace Core
{
	class Game : public std::enable_shared_from_this<Game>
	{
	private:
		std::shared_ptr<Window> m_window;
		bool m_isRunning = false;
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
		std::unique_ptr<ResourcesManager> m_resources;
		std::shared_ptr<Scene> m_activeScene;
		std::string m_activeSceneName;
#ifdef ZERO_USE_IMGUI
		ImGuiIO* pio = nullptr;
#endif
		float m_deltaTime = 0.0f;
		System::RenderSystem m_renderSystem;
		System::InputSystem m_inputSystem;
		System::AnimationSystem m_animationSystem;
		System::AudioSystem m_audioSystem;
		System::ActorComponentSystem m_actorSystem;

		// Timing state — must be members so Tick() can persist across Emscripten callbacks
		uint64_t m_perfFreq = 0;
		uint64_t m_prevCounter = 0;
		double m_accumulator = 0.0;

		static constexpr double FIXED_HZ  = 60.0;
		static constexpr double FIXED_DT  = 1.0 / FIXED_HZ;
		static constexpr double MAX_FRAME = 0.25;
		static constexpr int    MAX_STEPS = 5;

	public:
		Game();
		~Game();
	

		void Initialize();
		void Run();        // Desktop: blocking loop
		void StartLoop();  // Init timing state (call before Tick on Emscripten)
		void Tick();       // One frame of work — called by emscripten_set_main_loop

		std::shared_ptr<Window> GetWindow() const { return m_window; }

		// Scene management  
		void AddScene(const std::string& name, std::shared_ptr<Scene> scene);
		void SetActiveScene(const std::string& name);
		std::shared_ptr<Scene> GetActiveScene() const { return m_activeScene; }
		std::string GetActiveSceneName() const { return m_activeSceneName; }
		const float GetDeltaTime();
		System::RenderSystem& GetRenderSystem()  { return m_renderSystem; }
		System::InputSystem& GetInputSystem() { return m_inputSystem; }
		System::AudioSystem& GetAudioSystem()  { return m_audioSystem; }
		System::AnimationSystem& GetAnimationSystem() { return m_animationSystem; }
		ResourcesManager& GetResources()
		{
			return *m_resources;
		}
	};
}
