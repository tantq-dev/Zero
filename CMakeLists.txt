cmake_minimum_required(VERSION 3.16)
project(Zero LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SRC_FILES
        src/core/Game.cpp
        src/core/Scene.cpp
        src/core/ScenePlay.cpp
        src/core/Window.cpp
        src/main.cpp
        src/resources/ResourcesManager.cpp
        src/system/AnimationSystem.cpp
        src/system/InputSystem.cpp
        src/system/PhysicSystem.cpp
        src/system/RenderSystem.cpp
        src/utilities/Logger.cpp
        src/utilities/Vec2.cpp
        vendor/imgui/backends/imgui_impl_opengl3.cpp
        vendor/imgui/backends/imgui_impl_sdl3.cpp
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
)

add_executable(Zero ${SRC_FILES})

# Target-specific include directories (modern CMake approach)
target_include_directories(Zero PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/vendor
        ${PROJECT_SOURCE_DIR}/vendor/imgui
        ${PROJECT_SOURCE_DIR}/vendor/imgui/backends
        ${PROJECT_SOURCE_DIR}/src/config
        ${PROJECT_SOURCE_DIR}/src/utilities
        ${PROJECT_SOURCE_DIR}/src/core
        ${PROJECT_SOURCE_DIR}/src/resources
        ${PROJECT_SOURCE_DIR}/src/system
)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED CONFIG)

# Link libraries
target_link_libraries(Zero PRIVATE
        OpenGL::GL
        SDL3::SDL3
)

# Preprocessor definitions for Debug/Release
target_compile_definitions(Zero PRIVATE
        $<$<CONFIG:Debug>:_DEBUG;_CONSOLE>
        $<$<CONFIG:Release>:NDEBUG;_CONSOLE>
)
