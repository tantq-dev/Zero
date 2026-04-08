file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(ZeroGame ${GAME_SRC})

target_include_directories(ZeroGame PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ZeroGame PRIVATE
  Engine
  SDL3::SDL3
)

target_compile_definitions(ZeroGame PRIVATE
  $<$<CONFIG:Debug>:ZERO_DEBUG>
  $<$<CONFIG:Release>:ZERO_RELEASE>
)

if(EMSCRIPTEN)
  # ---- Emscripten settings ----
  set_target_properties(ZeroGame PROPERTIES SUFFIX ".html")

  target_link_options(ZeroGame PRIVATE
    "-sUSE_SDL=3"
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"

   
    "--preload-file=${CMAKE_SOURCE_DIR}/Engine/assets@/engine_assets"
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@/game_assets"
  )

else()
  # ---- Desktop: copy assets ----
  add_custom_command(TARGET ZeroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Engine/assets
            $<TARGET_FILE_DIR:ZeroGame>/engine_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:ZeroGame>/game_assets
    COMMENT "Copying Engine and Game assets"
  )

endif()