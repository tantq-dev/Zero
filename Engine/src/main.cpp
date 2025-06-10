////#include <stdio.h>
////#include "core/Game.h"
////#include "../vendored/imgui/imgui.h"
////#include "../vendored/imgui/backends/imgui_impl_opengl3.h"
////#include "../vendored/imgui/backends/imgui_impl_sdl3.h"
////#include <SDL3/SDL.h>
////#include <SDL3/SDL_opengl.h>
////#include <core/ScenePlay.h>
////
//////this main for testing purposes only, the main function is in the Game class
//////Should be removed in the future after build engine as lib
////int main(int, char**) {
////
////	return 0;
////}
//
//#pragma region DragAndDrop Example
////
////#include <SDL3/SDL.h>
////#include <vector>
////#include <string>
////#include <fstream>
////#include <sstream>
////#include <iostream>
////
////const int SCREEN_WIDTH = 800;
////const int SCREEN_HEIGHT = 600;
////const int GRID_SIZE = 10; // 10x10 grid (each cell is 80x60 pixels)
////const int CELL_WIDTH = SCREEN_WIDTH / GRID_SIZE;
////const int CELL_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;
////
////// Monster structure to hold name and position
////struct Monster {
////    std::string name;
////    float x, y;
////    bool isDragging;
////    Monster(std::string n, float posX, float posY) : name(n), x(posX), y(posY), isDragging(false) {}
////};
////
////// Wave structure to hold monsters for each wave
////struct Wave {
////    std::vector<Monster> monsters;
////};
////
////// Function to snap position to grid
////void snapToGrid(float& x, float& y) {
////    x = (x / CELL_WIDTH) * CELL_WIDTH + CELL_WIDTH / 2;
////    y = (y / CELL_HEIGHT) * CELL_HEIGHT + CELL_HEIGHT / 2;
////    if (x < 0) x = CELL_WIDTH / 2;
////    if (x > SCREEN_WIDTH) x = SCREEN_WIDTH - CELL_WIDTH / 2;
////    if (y < 0) y = CELL_HEIGHT / 2;
////    if (y > SCREEN_HEIGHT) y = SCREEN_HEIGHT - CELL_HEIGHT / 2;
////}
////
////// Function to draw 10x10 grid
////void drawGrid(SDL_Renderer* renderer) {
////    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray grid lines
////    for (int i = 0; i <= GRID_SIZE; ++i) {
////        // Vertical lines
////        SDL_RenderLine(renderer, i * CELL_WIDTH, 0, i * CELL_WIDTH, SCREEN_HEIGHT);
////        // Horizontal lines
////        SDL_RenderLine(renderer, 0, i * CELL_HEIGHT, SCREEN_WIDTH, i * CELL_HEIGHT);
////    }
////}
////
////
////
////// Function to export wave data to JSON
////void exportWaveToJson(const Wave& wave, int waveNumber) {
////    std::ofstream file("wave_" + std::to_string(waveNumber) + ".json");
////    if (!file.is_open()) {
////        std::cerr << "Failed to open file for wave " << waveNumber << std::endl;
////        return;
////    }
////
////    file << "[\n";
////    for (size_t i = 0; i < wave.monsters.size(); ++i) {
////        const Monster& m = wave.monsters[i];
////        file << "  {\"name\": \"" << m.name << "\", \"x\": " << m.x << ", \"y\": " << m.y << "}";
////        if (i < wave.monsters.size() - 1) file << ",";
////        file << "\n";
////    }
////    file << "]\n";
////    file.close();
////    std::cout << "Exported wave " << waveNumber << " to JSON\n";
////}
////
////int main(int argc, char* argv[]) {
////    // Initialize SDL
////    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
////        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
////        return 1;
////    }
////
////    // Create window
////    SDL_Window* window = SDL_CreateWindow("Monster Map Editor", SCREEN_WIDTH, SCREEN_HEIGHT, NULL);
////    if (!window) {
////        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
////        SDL_Quit();
////        return 1;
////    }
////
////    // Create renderer
////    SDL_Renderer* renderer = SDL_CreateRenderer(window,nullptr);
////    if (!renderer) {
////        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
////        SDL_DestroyWindow(window);
////        SDL_Quit();
////        return 1;
////    }
////
////    // Game variables
////    std::vector<Wave> waves;
////    int currentWave = 0;
////    waves.push_back(Wave()); // Start with wave 0
////    bool quit = false;
////    SDL_Event e;
////    Monster* draggedMonster = nullptr;
////
////    // Main loop
////    while (!quit) {
////        while (SDL_PollEvent(&e) != 0) {
////            if (e.type == SDL_EVENT_QUIT) {
////                quit = true;
////            }
////            else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
////                float x, y;
////                SDL_GetMouseState(&x, &y);
////                // Check if clicking a monster to drag
////                for (Monster& m : waves[currentWave].monsters) {
////                    if (x >= m.x - 20 && x <= m.x + 20 && y >= m.y - 20 && y <= m.y + 20) {
////                        m.isDragging = true;
////                        draggedMonster = &m;
////                        break;
////                    }
////                }
////            }
////            else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
////                if (draggedMonster) {
////                    float x, y;
////                    SDL_GetMouseState(&x, &y);
////                    snapToGrid(x, y);
////                    draggedMonster->x = x;
////                    draggedMonster->y = y;
////                    draggedMonster->isDragging = false;
////                    draggedMonster = nullptr;
////                }
////            }
////            else if (e.type == SDL_EVENT_MOUSE_MOTION && draggedMonster) {
////                SDL_GetMouseState(&draggedMonster->x, &draggedMonster->y);
////            }
////            else if (e.type == SDL_EVENT_KEY_DOWN) {
////                if (e.key.scancode == SDL_SCANCODE_N) { // 'n' to add new monster
////                    std::string name = "Monster" + std::to_string(waves[currentWave].monsters.size() + 1);
////                    float x = SCREEN_WIDTH / 2;
////                    float y = SCREEN_HEIGHT / 2;
////                    snapToGrid(x, y);
////                    waves[currentWave].monsters.push_back(Monster(name, x, y));
////                }
////                else if (e.key.scancode == SDL_SCANCODE_W) { // 'w' to switch wave
////                    exportWaveToJson(waves[currentWave], currentWave);
////                    currentWave++;
////                    if (currentWave >= static_cast<int>(waves.size())) {
////                        waves.push_back(Wave());
////                    }
////                    std::cout << "Switched to wave " << currentWave << std::endl;
////                }
////                else if (e.key.scancode == SDL_SCANCODE_S) { // 's' to save current wave
////                    exportWaveToJson(waves[currentWave], currentWave);
////                }
////            }
////        }
////
////        // Clear screen
////        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
////        SDL_RenderClear(renderer);
////
////        // Draw grid
////        drawGrid(renderer);
////
////        // Draw monsters as red circles
////        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
////        for (const Monster& m : waves[currentWave].monsters) {
////            for (int i = 0; i < 360; i++) {
////                float rad = i * 3.14159f / 180.0f;
////                int x = m.x + 20 * cos(rad);
////                int y = m.y + 20 * sin(rad);
////                SDL_RenderPoint(renderer, x, y);
////            }
////        }
////
////        // Present renderer
////        SDL_RenderPresent(renderer);
////    }
////
////    // Export all waves on exit
////    for (int i = 0; i < static_cast<int>(waves.size()); ++i) {
////        exportWaveToJson(waves[i], i);
////    }
////
////    // Cleanup
////    SDL_DestroyRenderer(renderer);
////    SDL_DestroyWindow(window);
////    SDL_Quit();
////    return 0;
////}
//
//#pragma endregion
////
//#pragma region DearImgui_Example
////
////
////// Dear ImGui: standalone example application for SDL3 + OpenGL
////// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
////
////// Learn about Dear ImGui:
////// - FAQ                  https://dearimgui.com/faq
////// - Getting Started      https://dearimgui.com/getting-started
////// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
////// - Introduction, links and more at the top of imgui.cpp
////
//#include "imgui.h"
//#include "imgui_impl_sdl3.h"
//#include "imgui_impl_opengl3.h"
//#include <stdio.h>
//#include <SDL3/SDL.h>
//#if defined(IMGUI_IMPL_OPENGL_ES2)
//#include <SDL3/SDL_opengles2.h>
//#else
//#include <SDL3/SDL_opengl.h>
//#endif
//
//#ifdef __EMSCRIPTEN__
//#include "../libs/emscripten/emscripten_mainloop_stub.h"
////#endif
//
//// Main code
//int main(int, char**)
//{
//
//	// Decide GL+GLSL versions
//#if defined(IMGUI_IMPL_OPENGL_ES2)
//	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
//	const char* glsl_version = "#version 100";
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//#elif defined(IMGUI_IMPL_OPENGL_ES3)
//	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
//	const char* glsl_version = "#version 300 es";
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//#elif defined(__APPLE__)
//	// GL 3.2 Core + GLSL 150
//	const char* glsl_version = "#version 150";
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
//#else
//	// GL 3.0 + GLSL 130
//	const char* glsl_version = "#version 130";
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//#endif
//
//	// Create window with graphics context
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
//
//	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
//	if (gl_context == nullptr)
//	{
//		printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
//		return -1;
//	}
//
//	SDL_GL_MakeCurrent(window, gl_context);
//	SDL_GL_SetSwapInterval(1); // Enable vsync
//	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
//	SDL_ShowWindow(window);
//
//	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//
//	// Setup Dear ImGui style
//	ImGui::StyleColorsDark();
//	//ImGui::StyleColorsLight();
//
//	// Setup Platform/Renderer backends
//	ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
//	ImGui_ImplOpenGL3_Init(glsl_version);
//
//
//	// Cleanup
//	// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplSDL3_Shutdown();
//	ImGui::DestroyContext();
//
//	SDL_GL_DestroyContext(gl_context);
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//
//	return 0;
//}
//#pragma endregion
