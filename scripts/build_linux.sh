#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Function to display error message and exit
function error {
    echo "Error: $1"
    exit 1
}

# Create and activate a virtual environment
echo "Creating and activating a virtual environment..."
python3 -m venv venv || error "Failed to create virtual environment."
source venv/bin/activate || error "Failed to activate virtual environment."

# Install development dependencies
echo "Installing development dependencies..."
pip install -r requirements-build.txt || error "Failed to install development dependencies."

# Clean the dist directory
echo "Cleaning the dist directory..."
rm -rf dist || error "Failed to clean the dist directory."

# Build the wheel package
echo "Building the wheel package..."
python -m build --wheel --no-isolation --config-setting=build-dir=./build || error "Failed to build the wheel package."

# Install the built wheel
echo "Installing the built wheel..."
pip install dist/*.whl || error "Failed to install the package from the dist directory."

# Test the installation
echo "Testing the installation..."
python -c "import pyamtrack; print(pyamtrack.converters.beta_from_energy(150))" || error "Failed to import pyamtrack or run the test script."

echo "Package built, installed, and tested successfully!"