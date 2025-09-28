# Sources
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(Engine STATIC ${ENGINE_SRC} "include/DefaultConfig.h")

# Public headers during build; change INSTALL_INTERFACE if you later add install rules
target_include_directories(Engine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendored>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

  $<INSTALL_INTERFACE:include>
)

# Dependencies
target_link_libraries(Engine PUBLIC
  SDL3::SDL3
  imgui           # assumes vendored/imgui defines target "imgui"
  assimp
  OpenGL::GL
  glm
  glad
)

# Warnings & common flags
if(MSVC)
  target_compile_options(Engine PRIVATE /W4 /permissive- /MP)
else()
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ImGui (vendored)
add_subdirectory(vendored/imgui)

target_compile_definitions(Engine PRIVATE
  $<$<CONFIG:Debug>:ZERO_DEBUG>
  $<$<CONFIG:Release>:ZERO_RELEASE>
)

# If you ever link this static lib into a shared lib, you may need:
# set_target_properties(Zero PROPERTIES POSITION_INDEPENDENT_CODE ON)
