#Engine CMakeList
project(Zero LANGUAGES CXX)

# Add ImGui subdirectory
add_subdirectory(vendored/imgui)

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(Zero STATIC ${ENGINE_SRC})

target_include_directories(Zero PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendored
)

target_link_libraries(Zero PUBLIC
    SDL3::SDL3
    imgui  # Add ImGui library
)

target_compile_definitions(Zero PRIVATE
    $<$<CONFIG:Debug>:ZERO_DEBUG>
    $<$<CONFIG:Release>:ZERO_RELEASE>
)
