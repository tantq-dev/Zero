cmake_minimum_required(VERSION 3.16)
project(ZeroProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Configure Assimp build options
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)  # Build as shared library (DLL)

# Add external libraries
add_subdirectory(Extern/SDL EXCLUDE_FROM_ALL)
add_subdirectory(Extern/assimp EXCLUDE_FROM_ALL)

# GLM is header-only, so just set up include path
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/Extern/glm)

# GLAD setup - CHECK THESE PATHS
if(EXISTS "${CMAKE_SOURCE_DIR}/Extern/glad/src/glad.c")
    message(STATUS "GLAD source found at ${CMAKE_SOURCE_DIR}/Extern/glad/src/glad.c")
    add_library(glad STATIC "${CMAKE_SOURCE_DIR}/Extern/glad/src/glad.c")
    target_include_directories(glad PUBLIC "${CMAKE_SOURCE_DIR}/Extern/glad/include")
    target_link_libraries(glad PUBLIC OpenGL::GL)
else()
    message(FATAL_ERROR "GLAD source not found at ${CMAKE_SOURCE_DIR}/Extern/glad/src/glad.c
    Please download GLAD from https://glad.dav1d.de/ and extract to Extern/glad/")
endif()

# Subprojects
add_subdirectory(Engine)
add_subdirectory(Game)