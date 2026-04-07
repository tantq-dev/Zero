cmake_minimum_required(VERSION 3.22)
project(ZeroProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# -------- Output directories --------
foreach(OUTPUTCONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_U)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_U} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_U} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_U} ${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG})
endforeach()

# -------- Detect Emscripten --------
if(EMSCRIPTEN)
    message(STATUS "Building with Emscripten")

    # Output HTML instead of .exe
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # Global compile flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=3")

    # Global link flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
        -sWASM=1
        -sALLOW_MEMORY_GROWTH=1
    ")
else()
    find_package(OpenGL REQUIRED)
endif()

# -------- SDL3 --------
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(SDL3)

# -------- GLM --------
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glm)

# -------- GLAD (desktop only) --------
if(NOT EMSCRIPTEN)
  FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
  )
  FetchContent_MakeAvailable(glad)

  if(TARGET glad)
    target_link_libraries(glad PUBLIC OpenGL::GL)
  endif()
endif()

# -------- Assimp (desktop only) --------
if(NOT EMSCRIPTEN)
  set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
  set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
  set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

  FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        master
  )
  FetchContent_MakeAvailable(assimp)
endif()

# -------- Subprojects --------
add_subdirectory(Engine)
add_subdirectory(Game)