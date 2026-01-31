#!/bin/bash
set -e

echo "=== Building Release Mode ==="
cmake --preset release
cmake --build --preset release

echo "=== Launching Release Application ==="
./build/release/app/metro_app