#!/bin/bash
set -e # Exit immediately if a command fails

echo "=== Building Debug Mode ==="
cmake --preset debug
cmake --build --preset debug

echo "=== Launching Application ==="
./build/debug/app/metro_app