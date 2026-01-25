set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core
    core/simulation/Simulation.cpp
    core/simulation/TickClock.cpp
    core/graph/Graph.cpp
    core/graph/passenger_state_machine.cpp
    core/graph/routing_cache.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
