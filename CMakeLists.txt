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

# -------- Threads fix --------
if(EMSCRIPTEN)
    message(STATUS "Building with Emscripten")
    add_library(Threads::Threads INTERFACE IMPORTED)
else()
    find_package(Threads REQUIRED)
endif()

# -------- Platform config --------
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # ✅ ONLY use these (safe for Ninja)
    add_compile_options(-sUSE_SDL=3 -sUSE_FREETYPE=1)
    add_link_options(-sWASM=1 -sALLOW_MEMORY_GROWTH=1)

    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
else()
    find_package(OpenGL REQUIRED)
endif()

# -------- SDL3 --------
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
)

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG main
)
if(EMSCRIPTEN)
    set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
    set(SDLTTF_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
endif()



FetchContent_MakeAvailable(SDL3 SDL3_ttf)

# -------- Subprojects --------
add_subdirectory(Engine)
add_subdirectory(Game)