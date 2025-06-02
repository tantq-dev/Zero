cmake_minimum_required(VERSION 3.16)
project(Zero VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Group source files by modules
set(CORE_FILES
        src/core/Game.cpp
        src/core/Scene.cpp
        src/core/ScenePlay.cpp
        src/core/Window.cpp
)

set(RESOURCES_FILES
        src/resources/ResourcesManager.cpp
)

set(SYSTEM_FILES
        src/system/AnimationSystem.cpp
        src/system/InputSystem.cpp
        src/system/PhysicSystem.cpp
        src/system/RenderSystem.cpp
)

set(UTILITIES_FILES
        src/utilities/Logger.cpp
        src/utilities/Vec2.cpp
)

set(VENDOR_FILES
        vendor/imgui/backends/imgui_impl_opengl3.cpp
        vendor/imgui/backends/imgui_impl_sdl3.cpp
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
)

# Main executable
add_executable(Zero
        src/main.cpp
        ${CORE_FILES}
        ${RESOURCES_FILES}
        ${SYSTEM_FILES}
        ${UTILITIES_FILES}
        ${VENDOR_FILES}
)

# Source groups for better IDE organization
source_group("Core" FILES ${CORE_FILES})
source_group("Resources" FILES ${RESOURCES_FILES})
source_group("Systems" FILES ${SYSTEM_FILES})
source_group("Utilities" FILES ${UTILITIES_FILES})
source_group("Vendor" FILES ${VENDOR_FILES})

# Target-specific include directories
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

# Preprocessor definitions
target_compile_definitions(Zero PRIVATE
        $<$<CONFIG:Debug>:_DEBUG;ZERO_DEBUG=1>
        $<$<CONFIG:Release>:NDEBUG;ZERO_RELEASE=1>
        ZERO_VERSION="${PROJECT_VERSION}"
)

# Asset handling - Copy assets directory to binary output directory
add_custom_command(TARGET Zero POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
        COMMENT "Copying assets to build directory"
)

# Set the working directory for debugging in IDEs
set_target_properties(Zero PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        XCODE_GENERATE_SCHEME TRUE
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

# Installation rules
install(TARGETS Zero
        RUNTIME DESTINATION bin
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets"
        DESTINATION bin
)

# Add testing support
include(CTest)
if(BUILD_TESTING)
    # Add a separate subdirectory for tests
    # add_subdirectory(tests)
endif()

# Generate compile_commands.json for tools like clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Print configuration summary
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "Zero Game Engine Configuration:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  C++ Compiler:      ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "--------------------------------------------------------------------------------")