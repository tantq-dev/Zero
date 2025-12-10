file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(ZeroGame ${GAME_SRC})

target_include_directories(ZeroGame PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ZeroGame PRIVATE Engine)

target_compile_definitions(ZeroGame PRIVATE
  $<$<CONFIG:Debug>:ZERO_DEBUG>
  $<$<CONFIG:Release>:ZERO_RELEASE>
)

# ---- Post-build: copy assets ----
add_custom_command(TARGET ZeroGame POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/Engine/assets
          $<TARGET_FILE_DIR:ZeroGame>/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:ZeroGame>/assets
  COMMENT "Copying Engine and Game assets to output directory"
)

# ---- Post-build: copy Assimp DLL (Windows + if shared) ----
if(WIN32)
  add_custom_command(TARGET ZeroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:assimp>
            $<TARGET_FILE_DIR:ZeroGame>
    COMMENT "Copying assimp runtime to output"
  )
endif()
