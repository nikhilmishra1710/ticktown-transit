#!/bin/bash
set -e

echo "=== Compiling Release Build ==="
cmake --preset release
cmake --build --preset release

echo "=== Packaging Artifacts ==="
# Create artifacts directory if it doesn't exist
mkdir -p artifacts

# Copy the executable to the artifacts folder
cp build/release/app/metro_app artifacts/

echo "Build successful! Binary located at: artifacts/metro_app"