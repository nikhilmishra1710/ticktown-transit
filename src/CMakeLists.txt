set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/4.5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

add_library(core
    core/simulation/Simulation.cpp
    core/simulation/TickClock.cpp
    core/graph/Graph.cpp
    core/graph/passenger_state_machine.cpp
    core/graph/routing_cache.cpp
    core/ui/DrawSnapshot.cpp
    core/ui/RaylibApp.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link raylib to core library (if core uses raylib)
target_link_libraries(core raylib)

# Platform-specific linking
if(WIN32)
    target_link_libraries(core winmm)
endif()
