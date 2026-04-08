# Sources
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(Engine STATIC ${ENGINE_SRC})

# Public headers during build; change INSTALL_INTERFACE if you later add install rules
target_include_directories(Engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendored>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

  $<INSTALL_INTERFACE:include>
)

# ---- ImGui option (auto-disabled for Emscripten) ----
option(ZERO_USE_IMGUI "Enable Dear ImGui integration" ON)
if(EMSCRIPTEN)
  set(ZERO_USE_IMGUI OFF CACHE BOOL "" FORCE)
endif()

# ImGui (vendored)
if(ZERO_USE_IMGUI)
  add_subdirectory(vendored/imgui)
endif()

# Dependencies
target_link_libraries(Engine PUBLIC
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

if(ZERO_USE_IMGUI)
  target_link_libraries(Engine PUBLIC imgui)
  target_compile_definitions(Engine PUBLIC ZERO_USE_IMGUI)
endif()

if(NOT EMSCRIPTEN)
  target_link_libraries(Engine PUBLIC
    OpenGL::GL
  )
endif()

# Warnings & common flags
if(MSVC)
  target_compile_options(Engine PRIVATE /W4 /permissive- /MP)
elseif(EMSCRIPTEN)
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
  target_link_options(Engine PUBLIC
    -sUSE_SDL=3
    -sWASM=1
    -sALLOW_MEMORY_GROWTH=1
  )
else()
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_definitions(Engine PRIVATE
  $<$<CONFIG:Debug>:ZERO_DEBUG>
  $<$<CONFIG:Release>:ZERO_RELEASE>
)

# If you ever link this static lib into a shared lib, you may need:
# set_target_properties(Zero PROPERTIES POSITION_INDEPENDENT_CODE ON)
