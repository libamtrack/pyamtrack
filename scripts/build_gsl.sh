#!/bin/bash

yum uninstall gsl-devel -y

GSL_VERSION=2.7
GSL_URL=https://ftp.gnu.org/gnu/gsl/gsl-${GSL_VERSION}.tar.gz
GSL_DIR=gsl-${GSL_VERSION}
GSL_TAR=${GSL_DIR}.tar.gz
wget -O ${GSL_TAR} ${GSL_URL}
tar -xvf ${GSL_TAR}
cd ${GSL_DIR}
./configure 
make -j
sudo make install