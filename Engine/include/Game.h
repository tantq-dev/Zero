#pragma once  
#include <memory>  
#include <string>  
#include <unordered_map>  
#include "Window.h"  
#include "Scene.h"  
#include "imgui.h" // Include the header file where ImGuiIO is defined  
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
		ImGuiIO* pio = nullptr; // Ensure ImGuiIO is properly declared and included  
		float m_deltaTime;
		std::shared_ptr<System::RenderSystem> m_renderSystem;
		std::shared_ptr<System::AnimationSystem> m_animationSystem;

	public:
		Game();
		~Game();
	

		void Initialize();
		void Run();

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
