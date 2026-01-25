include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Prevent overriding parent project compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(core_tests
determinism_tests.cpp 
graph_tests.cpp 
route_test.cpp 
train_simulation_test.cpp 
passenger_test.cpp 
passenger_heutristic.cpp    
transfer_test.cpp
routing_cache.cpp
)

target_link_libraries(core_tests
    PRIVATE
        core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(core_tests)
