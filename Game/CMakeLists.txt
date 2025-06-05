#Game CmakeLists
project(ZeroGame LANGUAGES CXX)

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(ZeroGame ${GAME_SRC})

target_include_directories(ZeroGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ZeroGame PRIVATE Zero)

add_custom_command(TARGET ZeroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Engine/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)
