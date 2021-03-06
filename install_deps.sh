#!/usr/bin/env bash

set -x # Print command traces before executing command

set -e # Exit immediately if a simple command exits with a non-zero status.

set -o pipefail # Return value of a pipeline as the value of the last command to
                # exit with a non-zero status, or zero if all commands in the
                # pipeline exit successfully.

libamtrack_compile() {
    git clone https://github.com/libamtrack/library.git
    cd library
    mkdir m4
    autoreconf --force --install
    ./configure --prefix=$HOME/usr
    make README
    make -j4
    make install
    cd ../
    ls $HOME/usr/lib/
    mkdir -p pyamtrack/libs/
    cp $HOME/usr/lib/libamtrack*so pyamtrack/libs/ || :
    cp $HOME/usr/lib/libamtrack*dylib pyamtrack/libs/ || :
}

apt_install() {
    PYTHON_VERSION=$1
    sudo apt-get -q update
    PYTHON2_CMD="sudo apt-get install -y libgsl0-dev gfortran"
    PYTHON3_CMD="sudo apt-get install -y libgsl0-dev gfortran"
    choose_python_version "$PYTHON_VERSION" "$PYTHON2_CMD" "$PYTHON3_CMD"
}


brew_install() {
    PYTHON_VERSION=$1
#   brew update
    PYTHON2_CMD="brew install gsl"
    PYTHON3_CMD="brew install gsl"
    choose_python_version "$PYTHON_VERSION" "$PYTHON2_CMD" "$PYTHON3_CMD"
}


yum_install() {
    PYTHON_VERSION=$1
    sudo yum update -y -q
    PYTHON2_CMD="sudo yum install -y -q TODO"
    PYTHON3_CMD="sudo yum install -y -q TODO"
    choose_python_version "$PYTHON_VERSION" "$PYTHON2_CMD" "$PYTHON3_CMD"
}


dnf_install() {
    PYTHON_VERSION=$1
    sudo dnf update -y -q
    PYTHON2_CMD="sudo dnf install -y -q TODO"
    PYTHON3_CMD="sudo dnf install -y -q TODO"
    choose_python_version "$PYTHON_VERSION" "$PYTHON2_CMD" "$PYTHON3_CMD"
}


choose_python_version() {
    PYTHON_VERSION=$1
    PYTHON2_CMD=$2
    PYTHON3_CMD=$3
    case "$PYTHON_VERSION" in
        py2*)
        echo "Py2: $PYTHON_VERSION"
        $PYTHON2_CMD
        ;;
        py3*)
        echo "Py3: $PYTHON_VERSION"
        $PYTHON3_CMD
        ;;
        *)
        echo "Unknown: $PYTHON_VERSION"
        ;;
     esac
}


# TODO - check if this would be good to discover system version
# found here: http://unix.stackexchange.com/questions/6345/how-can-i-get-distribution-name-and-version-number-in-a-simple-shell-script/6348
# check discussion below
function os_type
{
case `uname` in
  Linux )
     LINUX=1
     which yum && { echo centos; return; }
     which zypper && { echo opensuse; return; }
     which apt-get && { echo debian; return; }
     ;;
  Darwin )
     DARWIN=1
     ;;
  * )
     # Handle AmgiaOS, CPM, and modified cable modems here.
     ;;
esac
}


PYTHON_VERSION=$1

# Detect the platform (similar to $OSTYPE)
# Inspired by http://stackoverflow.com/questions/394230/detect-the-os-from-a-bash-script
# TODO improve furter
case "$OSTYPE" in
  solaris*)
    echo "Solaris: $OSTYPE"
    ;;
  darwin*) # Mac (OSX)
    echo "Mac: $OSTYPE"
    brew_install $PYTHON_VERSION
    ;;
  linux-gnu*) # Debian
    echo "Linux-gnu: $OSTYPE"
    apt_install $PYTHON_VERSION
#    os_type
#    lsb_release -si
#    lsb_release -sr
#    lsb_release -a
#    ls /etc/*release
    ;;
  linux*)
    echo "Linux: $OSTYPE"
    ;;
  bsd*)
    echo "BSD: $OSTYPE"
    ;;
  *)
    echo "Unknown: $OSTYPE"
    ;;
esac

libamtrack_compile