
#include "ScenePlay.h"
#include <stdexcept>
#include <SDL3/SDL.h>
#include "Logger.h"
#include "../vendored/imgui/imgui.h"
#include "../vendored/imgui/backends/imgui_impl_sdl3.h"
#include "../vendored/imgui/backends/imgui_impl_sdlrenderer3.h"
#include "Game.h"



namespace Core
{
	Game::Game() // Initialize the reference to ImGuiIO
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

			SDL_GLContext gl_context = SDL_GL_CreateContext(m_window->GetWindow().get());
			if (gl_context == nullptr)
			{
				printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
				return;
			}

			SDL_GL_MakeCurrent(m_window->GetWindow().get(), gl_context);
			SDL_GL_SetSwapInterval(1); // Enable vsync
			SDL_SetWindowPosition(m_window->GetWindow().get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			SDL_ShowWindow(m_window->GetWindow().get());

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

		}
		catch (const std::exception& e)
		{
			// Log and rethrow
			LOG_ERROR("Failed to initialize Game: " + std::string(e.what()));
			throw;
		}

	}

	void Game::Run()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		if (!m_activeScene)
		{
			throw std::runtime_error("No active scene set. Call SetActiveScene() before Run().");
		}

		
		m_activeScene->Initialize(*m_window->GetRenderer().get());
		m_isRunning = true;

		constexpr double FIXED_HZ = 60.0;
		constexpr double FIXED_DT = 1.0 / FIXED_HZ;        // seconds
		constexpr double MAX_FRAME = 0.25;                  // clamp 250 ms after hitches
		constexpr int    MAX_STEPS = 5;                     // avoid spiral of death


		Uint64 perfFreq = SDL_GetPerformanceFrequency();
		auto   toSec = [perfFreq](Uint64 ticks) { return static_cast<double>(ticks) / static_cast<double>(perfFreq); };

		Uint64 prev = SDL_GetPerformanceCounter();
		double accumulator = 0.0;

		SDL_Event event;
		

		while (m_isRunning)
		{

			Uint64 now = SDL_GetPerformanceCounter();
			
			double frameDt = toSec(now - prev);
			prev = now;

			if (frameDt > MAX_FRAME) frameDt = MAX_FRAME;
			accumulator += frameDt;
			m_deltaTime = static_cast<float>(frameDt);

			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();



			while (SDL_PollEvent(&event))
			{

				ImGui_ImplSDL3_ProcessEvent(&event);
				if (pio->WantCaptureMouse || pio->WantCaptureKeyboard) {
					continue;
				}

				m_activeScene->HandleInput(event);
				if (event.type == SDL_EVENT_QUIT)
				{
					m_isRunning = false;
					m_window->Close();
				}
			}

			// --- fixed updates ---
			int steps = 0;
			while (accumulator >= FIXED_DT && steps < MAX_STEPS) {
				m_activeScene->FixedUpdate(FIXED_DT);
				accumulator -= FIXED_DT;
				++steps;
			}

			// if we hit the max number of steps, we are probably in a spiral of death, so we just clamp the accumulator
			if (steps == MAX_STEPS && accumulator >= FIXED_DT) {
				accumulator = fmod(accumulator, FIXED_DT);
			}

		
			m_activeScene->Update(m_deltaTime);
			

			double alpha = accumulator / FIXED_DT;
			m_activeScene->Render(*m_window->GetRenderer().get(),alpha);
			m_activeScene->HandleUI(event);
			ImGui::Render();
			SDL_SetRenderScale(m_window->GetRenderer().get(), pio->DisplayFramebufferScale.x, pio->DisplayFramebufferScale.y);

			ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_window->GetRenderer().get());

			// Update active scene

			SDL_RenderPresent(m_window->GetRenderer().get());
			//SDL_SetRenderDrawColorFloat(m_window->GetRenderer().get(), clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			SDL_RenderClear(m_window->GetRenderer().get());
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

