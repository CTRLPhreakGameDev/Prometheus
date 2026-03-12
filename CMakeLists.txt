cmake_minimum_required(VERSION 4.0)
project(game VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_SYSTEM_RAYLIB "Prefer system-installed raylib" ON)

# --- Raylib ---
if (USE_SYSTEM_RAYLIB)
  find_package(raylib CONFIG QUIET)
endif()

if (NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
  )
  FetchContent_MakeAvailable(raylib)
endif()

# --- Your game ---
add_executable(game
  src/main.cpp
  src/core/game.cpp
  src/core/input/input.cpp
  src/gameplay/player.cpp
  src/gameplay/enemies/common.cpp
  src/gfx/cameraFollow.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
	include/
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Helpful warnings (tweak to taste)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Assets: copy to build output dir so running from build/ works ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
