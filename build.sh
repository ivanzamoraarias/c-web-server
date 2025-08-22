#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Project directory contents:"
ls -l 

# Create a build directory if it doesn't exist
mkdir -p build

# Enter the build directory
cd build

# Run cmake to generate build files
cmake ..

# Build the project
cmake --build .

echo "Build completed successfully."