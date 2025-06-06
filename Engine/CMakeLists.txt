#Engine CMakeList
project(ToolEngine LANGUAGES CXX)

# Add ImGui subdirectory
add_subdirectory(vendored/imgui)

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(ToolEngine STATIC ${ENGINE_SRC} "src/system/CameraSystem.h" "src/system/CameraSystem.cpp")

target_include_directories(ToolEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendored
)

target_link_libraries(ToolEngine PUBLIC
    SDL3::SDL3
    imgui  # Add ImGui library
)

target_compile_definitions(ToolEngine PRIVATE
    $<$<CONFIG:Debug>:TOOL_ENGINE_DEBUG>
    $<$<CONFIG:Release>:TOOL_ENGINE_RELEASE>
)
