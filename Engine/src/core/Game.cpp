
#include "ScenePlay.h"
#include <stdexcept>
#include "Logger.h"
#ifdef ZERO_USE_IMGUI
#include "../vendored/imgui/imgui.h"
#include "../vendored/imgui/backends/imgui_impl_sdl3.h"
#include "../vendored/imgui/backends/imgui_impl_sdlrenderer3.h"
#endif
#include "Game.h"
#include "EngineEvents.h"
#include "RendererFactory.h"
#include "IRenderer2D.h"
#include "DefaultConfig.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

//todo: decoupling Game from Backends (SDL, OpenGL, Vulkan, DirectX, Metal, etc)

namespace Core
{
	Game::Game()
	{
		m_window = std::make_shared<Window>();
		m_scenes.clear();
		m_activeScene = nullptr;
	}

	Game::~Game()
	{
		m_audioSystem.Shutdown();
		if (m_window) m_window->Close();
	}

	void Game::Initialize()
	{
		WindowConfig windowConfig;
		try
		{
			// Initialize window
			m_window->Initialize(windowConfig);
			m_audioSystem.Initialize();
			m_resources = std::make_unique<ResourcesManager>();

			m_renderSystem.SetRenderer(System::RendererFactory::CreateRenderer(m_window));
			m_renderSystem.SetResourcesManager(m_resources.get());

			//initial default resouce
			m_resources->GetOrLoadFont(EngieResources::DEFAULT_FONT, 24,m_renderSystem.GetRenderer());
#ifndef __EMSCRIPTEN__
			SDL_GLContext gl_context = SDL_GL_CreateContext(m_window->GetWindow().get());
			if (gl_context == nullptr)
			{
				printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
				return;
			}

			SDL_GL_MakeCurrent(m_window->GetWindow().get(), gl_context);
			SDL_GL_SetSwapInterval(1); // Enable vsync
#endif
			SDL_SetWindowPosition(m_window->GetWindow().get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			SDL_ShowWindow(m_window->GetWindow().get());

#ifdef ZERO_USE_IMGUI
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			pio = &ImGui::GetIO(); (void)&pio;
			pio->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			pio->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();
			//todo: remove dependency
			ImGui_ImplSDL3_InitForSDLRenderer(m_window->GetWindow().get(), m_window->GetRenderer().get());
			ImGui_ImplSDLRenderer3_Init(m_window->GetRenderer().get());
#endif

		}
		catch (const std::exception& e)
		{
			// Log and rethrow
			LOG_ERROR("Failed to initialize Game: " + std::string(e.what()));
			throw;
		}

	}

	void Game::StartLoop()
	{
		m_isRunning = true;
		m_perfFreq = m_window->GetPerformanceFrequency();
		m_prevCounter = m_window->GetPerformanceCounter();
		m_accumulator = 0.0;
	}

	void Game::Tick()
	{
		if (!m_isRunning) return;

		uint64_t currentCounter = m_window->GetPerformanceCounter();
		double frameTime = (double)(currentCounter - m_prevCounter) / m_perfFreq;
		m_prevCounter = currentCounter;

		if (frameTime > MAX_FRAME) frameTime = MAX_FRAME;
		m_accumulator += frameTime;
		m_eventSystem.Publish(Events::FrameStarted{ frameTime });

#ifdef ZERO_USE_IMGUI
		// TODO: Implement ImGui abstraction if needed
#endif

		m_isRunning = m_window->ProcessEvents([this](SDL_Event& event) {
			m_inputSystem.HandleInput(event);
		});

		// Snapshot UI mouse state before any scene logic runs.
		m_uiSystem.BeginFrame(m_inputSystem, m_renderSystem.GetRenderer());

		if (m_activeScene) {
			m_activeScene->HandleInput();
		}

		int steps = 0;
		m_activeScene->Update(frameTime);
		m_animationSystem.Update(m_activeScene->GetWorld()->Registry, (float)frameTime, *m_resources);
		m_actorSystem.Update(frameTime, m_activeScene->GetWorld()->Registry);
		while (m_accumulator >= FIXED_DT && steps < MAX_STEPS)
		{
			m_deltaTime = (float)FIXED_DT;
			m_accumulator -= FIXED_DT;
			m_eventSystem.Publish(Events::FixedUpdate{ m_deltaTime });
			m_activeScene->FixedUpdate(m_deltaTime);
			m_actorSystem.FixedUpdate(frameTime, m_activeScene->GetWorld()->Registry);

			steps++;
		}

		// ===== RENDER PIPELINE =====
		auto renderer = m_window->GetRenderer();

		m_renderSystem.GetRenderer().BeginFrame();
		
		// Render entities from the registry
		m_renderSystem.Update(m_activeScene->GetWorld()->Registry);

		// Render scene-specific UI and overlays
		m_activeScene->Render(m_renderSystem.GetRenderer());

		m_renderSystem.GetRenderer().CallRender();

		// ---- UI pass (screen-space, on top of world) ----
		m_activeScene->HandleUI(m_uiSystem);
		m_uiSystem.Flush(m_renderSystem.GetRenderer());

#ifdef ZERO_USE_IMGUI
		// TODO: Implement ImGui abstraction if needed
#endif

		// EndFrame flushes UI queues then calls SDL_RenderPresent.
		m_renderSystem.GetRenderer().EndFrame(0, 0);

#ifndef __EMSCRIPTEN__
		uint64_t endCounter = m_window->GetPerformanceCounter();
		double elapsed = (double)(endCounter - currentCounter) / m_perfFreq;
		if (elapsed < FIXED_DT)
		{
			m_window->Delay((uint32_t)((FIXED_DT - elapsed) * 1000.0));
		}
#endif
		m_eventSystem.Publish(Events::FrameEnded{ frameTime });
		m_eventSystem.DispatchQueued();
		if (m_activeScene)
		{
			m_activeScene->GetWorld()->GetEvents().DispatchQueued();
		}
		m_inputSystem.PostUpdate();
	}

	void Game::Run()
	{
		if (!m_activeScene)
		{
			throw std::runtime_error("No active scene set. Call SetActiveScene() before Run().");
		}
		
		StartLoop();

		while (m_isRunning)
		{
			Tick();
		}
	}

	void Game::AddScene(const std::string& name, std::shared_ptr<Scene> scene)
	{
		if (m_scenes.find(name) != m_scenes.end())
		{
			throw std::runtime_error("Scene with name " + name + " already exists.");
		}

		
		scene->SetGame(weak_from_this());
		m_scenes[name] = std::move(scene);
	}

	void Game::SetActiveScene(const std::string& name)
	{
		auto it = m_scenes.find(name);
		if (it != m_scenes.end())
		{
			m_eventSystem.Publish(Events::SceneChanging{ m_activeSceneName, name });
			if (m_activeScene)
			{
				m_activeScene->OnSceneUnload();
			}
			m_activeScene = it->second;
			m_activeSceneName = name;
			m_activeScene->Initialize();
			m_eventSystem.Publish(Events::SceneChanged{ m_activeSceneName, m_activeScene });
		}
		else
		{
			throw std::runtime_error("Scene not found: " + name);
		}
	}
	const float Game::GetDeltaTime() {
		return m_deltaTime;
	}
}

