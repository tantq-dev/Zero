#include "Game.h"
#include "ScenePlay.h"
#include <stdexcept>
#include <SDL3/SDL.h>
#include "utilities/Logger.h"
#include "config/ApplicationConfig.h"
#include "../vendored/imgui/imgui.h"
#include "../vendored/imgui/backends/imgui_impl_sdl3.h"
#include "../vendored/imgui/backends/imgui_impl_sdlrenderer3.h"



namespace Core
{
	Game::Game() // Initialize the reference to ImGuiIO
	{
		if (SDL_Init(SDL_INIT_VIDEO) == 0)
		{
			throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
		}

		m_window = std::make_shared<Window>();
	}

	Game::~Game()
	{
		SDL_Quit();
	}

	void Game::Initialize()
	{

		try
		{
			// Initialize window
			m_window->Initialize(
				ApplicationConfig::APP_NAME,
				ApplicationConfig::DEFAULT_WINDOW_WIDTH,
				ApplicationConfig::DEFAULT_WINDOW_HEIGHT,
				SDL_WINDOW_OPENGL
			);

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
		SDL_Event event;
		Uint32 previousTicks = SDL_GetTicks();
		float deltaTime = 0.0f;

		const int FRAME_DELAY = 1000 / ApplicationConfig::DEFAULT_MAX_FPS;

		while (m_isRunning)
		{

			Uint32 frameStart = SDL_GetTicks();

			Uint32 currentTicks = SDL_GetTicks();
			deltaTime = (currentTicks - previousTicks) / 1000.0f;
			previousTicks = currentTicks;

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

			m_activeScene->Update(deltaTime);
			m_activeScene->Render(*m_window->GetRenderer().get());

			m_activeScene->HandleUI(event);
			ImGui::Render();
			SDL_SetRenderScale(m_window->GetRenderer().get(), pio->DisplayFramebufferScale.x, pio->DisplayFramebufferScale.y);

			ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_window->GetRenderer().get());

			// Update active scene

			SDL_RenderPresent(m_window->GetRenderer().get());
			SDL_SetRenderDrawColorFloat(m_window->GetRenderer().get(), clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			SDL_RenderClear(m_window->GetRenderer().get());


			// Frame rate capping
			Uint32 frameTime = SDL_GetTicks() - frameStart;
			if (frameTime < FRAME_DELAY)
			{
				SDL_Delay(FRAME_DELAY - frameTime);
			}

		}
	}

	void Game::AddScene(const std::string& name, std::shared_ptr<Scene> scene)
	{
		m_scenes[name] = scene;
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
}

