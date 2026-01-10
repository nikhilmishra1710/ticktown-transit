add_library(core
    core/simulation/Simulation.cpp
    core/simulation/TickClock.cpp
    core/graph/Graph.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
