# Collect all test files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(unit_tests ${TEST_SOURCES})

# Link GTest and metro_core
target_link_libraries(unit_tests PRIVATE gtest_main metro_core)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)