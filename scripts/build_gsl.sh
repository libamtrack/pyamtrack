#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Detect OS
OS="$(uname -s)"

# Function to uninstall GSL
uninstall_gsl() {
    echo "Checking for existing GSL installation..."
    if command -v yum &>/dev/null; then
        if rpm -q gsl-devel &>/dev/null; then
            echo "Removing gsl-devel using yum..."
            yum remove gsl-devel -y
        else
            echo "gsl-devel is not installed. Skipping uninstallation."
        fi
    elif command -v apk &>/dev/null; then
        if apk info --installed gsl &>/dev/null; then
            echo "Removing GSL using apk..."
            apk del gsl gsl-dev
        else
            echo "gsl is not installed. Skipping uninstallation."
        fi
    else
        echo "No supported package manager found for GSL uninstallation."
    fi
}

# Function to install wget
install_wget() {
    if command -v wget &>/dev/null; then
        echo "wget is already installed."
        return
    fi

    echo "Installing wget..."
    if command -v yum &>/dev/null; then
        yum install -y wget
    elif command -v apk &>/dev/null; then
        apk add wget
    elif [[ "$OS" == "Darwin" ]]; then
        if ! command -v brew &>/dev/null; then
            echo "Homebrew is not installed. Please install Homebrew first: https://brew.sh/"
            exit 1
        fi
        brew install wget
    else
        echo "No supported package manager found to install wget."
        exit 1
    fi
}

# Uninstall gsl-devel if installed
uninstall_gsl

# Install wget if not already present
install_wget

# Define GSL version
GSL_VERSION=2.8
GSL_URL="https://ftp.gnu.org/gnu/gsl/gsl-${GSL_VERSION}.tar.gz"
GSL_DIR="gsl-${GSL_VERSION}"
GSL_TAR="${GSL_DIR}.tar.gz"

# Download and extract GSL
echo "Downloading GSL version ${GSL_VERSION}..."
wget -q -O "${GSL_TAR}" "${GSL_URL}"

echo "Extracting GSL..."
tar -xf "${GSL_TAR}"

# Build and install GSL
cd "${GSL_DIR}"
echo "Configuring GSL..."
./configure

echo "Building GSL using all available CPU cores..."
make -j

echo "Installing GSL..."
make install

echo "GSL installation completed successfully!"
