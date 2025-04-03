#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Function to display error message and exit
function error {
    echo "Error: $1"
    exit 1
}

# Clean the dist directory
echo "Cleaning the dist directory..."
rm -rf dist || error "Failed to clean the dist directory."

# Uninstall the existing package
echo "Uninstalling the existing pyamtrack package..."
pip uninstall pyamtrack --yes || error "Failed to uninstall the pyamtrack package."

# Build the package
echo "Building the package..."
python -m build --wheel || error "Failed to build the package."

# Install the newly built package
echo "Installing the newly built package..."
pip install dist/*.whl || error "Failed to install the package from the dist directory."

# Check with pip show
pip show -f pyamtrack

# Test the installation
echo "Testing the installation..."
python -c "import pyamtrack; print(dir(pyamtrack)); print(pyamtrack.beta_from_energy(2));" || error "Failed to import pyamtrack or run the test script."

python -c "import pyamtrack; print(pyamtrack.electron_range(100));" || error "Failed to import pyamtrack or run the test script."

python -c "import pyamtrack; print(pyamtrack.__version__);" || error "Failed to import pyamtrack or run the test script."

# Run pytest on the tests directory
echo "Running pytest on the tests directory..."
pytest tests/ || error "Pytest failed."

echo "Package built, installed, and tested successfully!"