cmake_minimum_required(VERSION 3.20)
project(metro_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable coverage (Default OFF)
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

include(FetchContent)

# 1. Fetch Raylib (For UI)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

# 2. Fetch GoogleTest (For Tests)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Prevent GTest from overriding parent compiler settings (Windows fix)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# 3. Add Subdirectories
add_subdirectory(src)
add_subdirectory(app)

enable_testing()
add_subdirectory(tests)