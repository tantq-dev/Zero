# ImGui CMake configuration
project(imgui LANGUAGES CXX)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Find OpenGL package
find_package(OpenGL REQUIRED)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    OpenGL::GL  # Add OpenGL library linkage
)

