
#include "ScenePlay.h"
#include <stdexcept>
#include <SDL3/SDL.h>
#include "Logger.h"
#ifdef ZERO_USE_IMGUI
#include "../vendored/imgui/imgui.h"
#include "../vendored/imgui/backends/imgui_impl_sdl3.h"
#include "../vendored/imgui/backends/imgui_impl_sdlrenderer3.h"
#endif
#include "Game.h"
#include <SDLRenderer2D.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

//todo: decoupling Game from Backends (SDL, OpenGL, Vulkan, DirectX, Metal, etc)

namespace Core
{
	Game::Game()
	{
		if (SDL_Init(SDL_INIT_VIDEO) == 0)
		{
			throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
		}

		m_window = std::make_shared<Window>();
		m_scenes.clear();
		m_activeScene = nullptr;
	}

	Game::~Game()
	{
		SDL_Quit();
	}

	void Game::Initialize()
	{
		WindowConfig windowConfig;
		try
		{
			// Initialize window
			m_window->Initialize(windowConfig);

			m_renderSystem = std::make_shared<System::RenderSystem>(std::make_shared<SDLRenderer>(m_window->GetRenderer()));
			m_animationSystem = std::make_shared<System::AnimationSystem>();

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
		m_perfFreq = SDL_GetPerformanceFrequency();
		m_prevCounter = SDL_GetPerformanceCounter();
		m_accumulator = 0.0;
	}

	void Game::Tick()
	{
		if (!m_isRunning) return;

		auto toSec = [this](Uint64 ticks) { return static_cast<double>(ticks) / static_cast<double>(m_perfFreq); };

		Uint64 now = SDL_GetPerformanceCounter();
		
		double frameDt = toSec(now - m_prevCounter);
		m_prevCounter = now;

		if (frameDt > MAX_FRAME) frameDt = MAX_FRAME;
		m_accumulator += frameDt;
		m_deltaTime = static_cast<float>(frameDt);

		SDL_Event event{};

#ifdef ZERO_USE_IMGUI
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
#endif

		while (SDL_PollEvent(&event))
		{

#ifdef ZERO_USE_IMGUI
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (pio->WantCaptureMouse || pio->WantCaptureKeyboard) {
				continue;
			}
#endif

			m_activeScene->HandleInput(event);
			if (event.type == SDL_EVENT_QUIT)
			{
				m_isRunning = false;
				m_window->Close();
#ifdef __EMSCRIPTEN__
				emscripten_cancel_main_loop();
#endif
			}
		}

		// --- fixed updates ---
		int steps = 0;
		while (m_accumulator >= FIXED_DT && steps < MAX_STEPS) {
			m_activeScene->FixedUpdate(FIXED_DT);
			m_accumulator -= FIXED_DT;
			++steps;
		}

		// if we hit the max number of steps, we are probably in a spiral of death, so we just clamp the accumulator
		if (steps == MAX_STEPS && m_accumulator >= FIXED_DT) {
			m_accumulator = fmod(m_accumulator, FIXED_DT);
		}

	
		m_activeScene->Update(m_deltaTime);
		m_animationSystem->Update(m_activeScene->GetRegistry(), m_deltaTime);

		double alpha = m_accumulator / FIXED_DT;
		(void)alpha; // Suppress unused variable warning
		m_renderSystem->Update(m_activeScene->GetRegistry());
		m_activeScene->HandleUI(event);

#ifdef ZERO_USE_IMGUI
		ImGui::Render();
		SDL_SetRenderScale(m_window->GetRenderer().get(), pio->DisplayFramebufferScale.x, pio->DisplayFramebufferScale.y);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_window->GetRenderer().get());
#endif

		// Update active scene

		SDL_RenderPresent(m_window->GetRenderer().get());
		//SDL_SetRenderDrawColorFloat(m_window->GetRenderer().get(), clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		SDL_RenderClear(m_window->GetRenderer().get());
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
			m_activeScene = it->second;
			m_activeSceneName = name;
			m_activeScene->Initialize();
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

