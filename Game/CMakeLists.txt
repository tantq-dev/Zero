cmake_minimum_required(VERSION 3.16)
project(ZeroGame LANGUAGES C CXX)

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(ZeroGame ${GAME_SRC})

target_include_directories(ZeroGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link to the Engine (which links to Assimp)
target_link_libraries(ZeroGame PRIVATE Zero)

target_compile_definitions(ZeroGame PRIVATE
    $<$<CONFIG:Debug>:ZERO_DEBUG>
    $<$<CONFIG:Release>:ZERO_RELEASE>
)

# Copy assets to output directory
add_custom_command(TARGET ZeroGame POST_BUILD
    # Copy Engine assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Engine/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets

    # Copy Game assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

# Copy the Assimp DLL to the output directory
add_custom_command(TARGET ZeroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:assimp>
    $<TARGET_FILE_DIR:ZeroGame>
)