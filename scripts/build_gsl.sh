#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Function to uninstall gsl-devel package if it's installed
uninstall_gsl_devel() {
    if command -v yum &>/dev/null; then
        if rpm -q gsl-devel &>/dev/null; then
            yum remove gsl-devel -y
        else
            echo "gsl-devel is not installed. Skipping uninstallation."
        fi
    elif command -v apk &>/dev/null; then
        if apk info --installed gsl &>/dev/null; then
            apk del gsl gsl-dev
        else
            echo "gsl is not installed. Skipping uninstallation."
        fi
    else
        echo "Neither yum nor apk is available. Cannot uninstall GSL."
        exit 1
    fi
}


# Function to install wget
install_wget() {
    if command -v yum &>/dev/null; then
        yum install -y wget
    elif command -v apk &>/dev/null; then
        apk add wget
    else
        echo "Neither yum nor apk is available. Cannot install wget."
        exit 1
    fi
}

# Uninstall gsl-devel package if it's installed
uninstall_gsl_devel

# Install wget if it's not already installed
install_wget


# Define the version of GSL to install
GSL_VERSION=2.7
# Define the URL to download GSL
GSL_URL=https://ftp.gnu.org/gnu/gsl/gsl-${GSL_VERSION}.tar.gz
# Define the directory name after extraction
GSL_DIR=gsl-${GSL_VERSION}
# Define the tarball name
GSL_TAR=${GSL_DIR}.tar.gz

# Download the GSL tarball
wget -O ${GSL_TAR} ${GSL_URL}

# Extract the tarball
tar -xvf ${GSL_TAR}

# Change to the GSL directory
cd ${GSL_DIR}

# Configure the GSL build
./configure 

# Build GSL using all available CPU cores
make -j

# Install the built GSL
make install