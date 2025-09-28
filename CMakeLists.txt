cmake_minimum_required(VERSION 3.22)
project(ZeroProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories (handle multi-config too)
foreach(OUTPUTCONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_U)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_U} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_U} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_U} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach()

find_package(OpenGL REQUIRED)

# -------- External deps --------
# SDL (assumes Extern/SDL provides SDL3::SDL3 via its CMake)
add_subdirectory(Extern/SDL EXCLUDE_FROM_ALL)

# GLM (header-only)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/Extern/glm)

# GLAD
if(EXISTS "${CMAKE_SOURCE_DIR}/Extern/glad/src/glad.c")
  add_library(glad STATIC "${CMAKE_SOURCE_DIR}/Extern/glad/src/glad.c")
  target_include_directories(glad PUBLIC "${CMAKE_SOURCE_DIR}/Extern/glad/include")
  target_link_libraries(glad PUBLIC OpenGL::GL)
else()
  message(FATAL_ERROR "GLAD not found at Extern/glad/src/glad.c")
endif()

# Assimp — limit BUILD_SHARED_LIBS scope to just this subdir
# Save previous setting
get_property(_prev_bsl GLOBAL PROPERTY GLOBAL_BUILD_SHARED_LIBS_SET)
if(NOT _prev_bsl)
  set(_orig_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
# Choose ON for DLLs, OFF for static (simpler deployment)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

add_subdirectory(Extern/assimp EXCLUDE_FROM_ALL)

# Restore previous BUILD_SHARED_LIBS if it existed
if(DEFINED _orig_BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ${_orig_BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)
endif()

# -------- Subprojects --------
add_subdirectory(Engine)
add_subdirectory(Game)
