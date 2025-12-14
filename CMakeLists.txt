cmake_minimum_required(VERSION 3.20)
project(metro_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tests)
enable_testing()
