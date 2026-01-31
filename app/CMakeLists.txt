add_executable(metro_app main.cpp)

# Link against the UI library (which transitively links core)
target_link_libraries(metro_app PRIVATE metro_ui)