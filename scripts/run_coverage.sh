#!/bin/bash
set -e # Exit immediately if a command fails

# 1. Clean previous coverage data (optional)
rm -rf build/coverage

# 2. Build and Test
cmake --preset coverage
cmake --build --preset coverage
ctest --preset coverage-test

# 3. Capture & Clean
echo "Generating Coverage Report..."
lcov --capture --directory build/coverage --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/_deps/*' --output-file coverage_clean.info --ignore-errors unused

# 4. Generate HTML
genhtml coverage_clean.info --output-directory coverage_html
echo "Done! Open coverage_html/index.html to view report."