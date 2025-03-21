#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Uninstall gsl-devel package if it's installed
yum remove gsl-devel -y

# Install wget if it's not already installed
yum install -y wget

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