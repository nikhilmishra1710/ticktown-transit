add_executable(metro_app main.cpp)

# Link against the UI library (which transitively links core and json)
target_link_libraries(metro_app PRIVATE metro_ui)

# --- NEW: Copy assets to the build directory ---
add_custom_command(TARGET metro_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:metro_app>/assets"
    COMMENT "Copying levels and assets to build directory"
)