#!/bin/bash
set -e # Exit immediately if a command fails

fresh=false
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh=true ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ "$fresh" = true ]; then
    rm -rf build/debug
fi

echo "=== Building Test Mode ==="
cmake --preset test
cmake --build --preset test

echo "=== Launching Test ==="
ctest --preset test