/**
 * Zero Engine - 3D Model Viewer
 * Created by: tantq-dev
 * Last Updated: 2025-06-30
 *
 * This program loads and renders 3D models using Zero's Renderer3D
 */

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/Renderer3D.h>
#include <resources/ModelImporter.h>

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <memory>

using namespace Zero;

// Main application class
class Application {
public:
    Application() : running(false), window(nullptr) {}

    // Initialize application
    bool Init(int width, int height) {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) == 0) {  // Fixed comparison, should check for non-zero (error)
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Create window
        window = SDL_CreateWindow(
            "Zero Engine Model Viewer",
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );

        if (!window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Initialize renderer
        renderer = std::make_unique<Renderer3D>(window, width, height);
        if (!renderer) {
            std::cerr << "Failed to create renderer" << std::endl;
            return false;
        }

        // Initialize resources
        if (!InitResources()) {
            std::cerr << "Failed to initialize resources" << std::endl;
            return false;
        }

        windowWidth = width;
        windowHeight = height;
        running = true;
        return true;
    }

    // Run the application
    void Run() {
        // Timing variables
        auto lastFrame = std::chrono::high_resolution_clock::now();

        // Main loop
        while (running) {
            // Calculate delta time
            auto currentFrame = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentFrame - lastFrame).count();
            lastFrame = currentFrame;

            // Process input
            ProcessInput(deltaTime);

            // Update game state
            Update(deltaTime);

            // Render
            Render();
        }
    }

    // Clean up resources
    void Cleanup() {
        // Clean up resources in reverse order
        renderer.reset();

        if (window) {
            SDL_DestroyWindow(window);
        }

        SDL_Quit();
    }

private:
    bool running;
    SDL_Window* window;
    int windowWidth, windowHeight;

    // Resources
    std::unique_ptr<Renderer3D> renderer;
    std::shared_ptr<Mesh> modelMesh;
    Camera camera;
    Transform modelTransform;
    float modelRotation = 0.0f;

    // Initialize resources
    bool InitResources() {
        // Set up camera
        camera.position = glm::vec3(0.0f, 0.0f, 15.0f);
        camera.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        camera.fov = 45.0f;

        // Set initial model transform
        modelTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        modelTransform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        modelTransform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

        // Load model mesh
        ModelLoader loader;
        modelMesh = loader.LoadModel("assets/models/tree.fbx");
        if (!modelMesh) {
            std::cerr << "Failed to load model" << std::endl;
            return false;
        }

        // Set up the renderer camera
        renderer->SetCamera(camera);

        return true;
    }

    // Process input
    void ProcessInput(float deltaTime) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
         
            else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                // No need to call glViewport directly - the renderer handles this
            }
        }

      
        // Update camera in renderer
        renderer->SetCamera(camera);
    }

    // Update game state
    void Update(float deltaTime) {
        // Rotate the model
        modelRotation += deltaTime * 45.0f; // 45 degrees per second
        if (modelRotation >= 360.0f) {
            modelRotation -= 360.0f;
        }

        // Update model transform
        modelTransform.rotation.y = modelRotation;
    }

    // Render scene
    void Render() {
        // Clear the framebuffer
        renderer->Clear();

        // Render the model
        renderer->RenderMesh(*modelMesh, modelTransform);

        // Present the rendered image
        renderer->Present();
    }
};

int main(int argc, char* argv[]) {
    // Create application
    Application app;

    // Initialize with 1024x768 window
    if (!app.Init(1024, 768)) {
        std::cerr << "Application initialization failed!" << std::endl;
        return 1;
    }

    // Run the application
    app.Run();

    // Clean up
    app.Cleanup();

    return 0;
}