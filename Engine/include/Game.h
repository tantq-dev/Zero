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
#include <AnimationSystem.h>

#define DEFAULT_FIXED_UPDATE_FRAME 60

namespace Core
{
	class Game : public std::enable_shared_from_this<Game>
	{
	private:
		std::shared_ptr<Window> m_window;
		bool m_isRunning = false;
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
		std::shared_ptr<Scene> m_activeScene;
		std::string m_activeSceneName;
#ifdef ZERO_USE_IMGUI
		ImGuiIO* pio = nullptr;
#endif
		float m_deltaTime = 0.0f;
		std::shared_ptr<System::RenderSystem> m_renderSystem;
		std::shared_ptr<System::AnimationSystem> m_animationSystem;

		// Timing state — must be members so Tick() can persist across Emscripten callbacks
		Uint64 m_perfFreq = 0;
		Uint64 m_prevCounter = 0;
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
		std::shared_ptr<System::RenderSystem> GetRenderSystem() const { return m_renderSystem; }
	};
}
