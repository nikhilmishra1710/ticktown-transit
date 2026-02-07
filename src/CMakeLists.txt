# --- LIBRARY 1: METRO CORE (Logic only) ---
# Collect all source files in core/ subdirectories
file(GLOB_RECURSE CORE_SOURCES "core/*.cpp")

add_library(metro_core ${CORE_SOURCES})

target_link_libraries(metro_core PUBLIC nlohmann_json::nlohmann_json)
# Include directories so other targets can see headers in src/core
target_include_directories(metro_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Apply Coverage Flags ONLY to metro_core
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(metro_core PRIVATE --coverage -O0 -g)
        target_link_options(metro_core PUBLIC --coverage)
        message(STATUS "Coverage enabled for metro_core")
    endif()
endif()

# --- LIBRARY 2: METRO UI (Raylib / Visualization) ---
# Collect all source files in ui/ subdirectories
file(GLOB_RECURSE UI_SOURCES "ui/*.cpp")

add_library(metro_ui ${UI_SOURCES})

# UI depends on Core (to read simulation state) AND Raylib
target_link_libraries(metro_ui PUBLIC metro_core raylib)
target_include_directories(metro_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})