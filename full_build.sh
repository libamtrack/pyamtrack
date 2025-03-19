#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status
set -x  # Print each command before executing it

# Define installation directory
INSTALL_DIR=$(pwd)/usr

# Remove any previous build and installation directories
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

if [ -d "$INSTALL_DIR" ]; then
    echo "Recreating ${INSTALL_DIR} directory..."
    rm -rf "$INSTALL_DIR"
fi

mkdir --parents $INSTALL_DIR

# Create and enter the build directory
mkdir build && cd build

# Configure the build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"

# Build the project
cmake --build . --parallel

# Install the compiled files
cmake --install .

echo "Build and installation completed successfully."

echo "Artifacts available at: $INSTALL_DIR"