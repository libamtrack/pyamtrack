cmake_minimum_required(VERSION 3.16)
project(pyamtrack VERSION 0.1.0 LANGUAGES CXX C)

# Set C++ standard to C++17 and enforce it
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable position-independent code (PIC) for building shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Adjust visibility settings for building Python packages
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)

# Ensure the symbols are not hidden
add_compile_options(-fvisibility=default)

# Fetch libamtrack library from GitHub
include(FetchContent)
FetchContent_Declare(
    libamtrack
    GIT_REPOSITORY https://github.com/libamtrack/library.git
    GIT_TAG master
)

# Disable unnecessary examples and tests from libamtrack
# (they require getopt, which is not needed here and adds extra dependencies)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")

# Set GSL paths for Windows using VCPKG environment variable
if (WIN32)
    # Check if VCPKG_INSTALLED_DIR environment variable is set
    if (NOT DEFINED ENV{VCPKG_INSTALLED_DIR})
        message(FATAL_ERROR "VCPKG_INSTALLED_DIR environment variable is not set.")
    endif()

    message(STATUS "VCPKG_INSTALLED_DIR (env variable): $ENV{VCPKG_INSTALLED_DIR}")

    # Set GSL include and library paths for VCPKG installation
    set(GSL_INCLUDE_DIR "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/include")
    set(GSL_LIBRARY "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/lib/gsl.lib")
    set(GSL_CBLAS_LIBRARY "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/lib/gslcblas.lib")
    message(STATUS "Using GSL include directory: ${GSL_INCLUDE_DIR}")
    message(STATUS "Using GSL library: ${GSL_LIBRARY}")
    message(STATUS "Using GSL CBLAS library: ${GSL_CBLAS_LIBRARY}")

    # Manually create GSL imported targets since they are not automatically available
    # Configure the GSL targets with include and library paths
    add_library(GSL::gsl UNKNOWN IMPORTED)
    set_target_properties(GSL::gsl PROPERTIES
        IMPORTED_LOCATION "${GSL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GSL_INCLUDE_DIR}"
    )
    add_library(GSL::gslcblas UNKNOWN IMPORTED)
    set_target_properties(GSL::gslcblas PROPERTIES
        IMPORTED_LOCATION "${GSL_CBLAS_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GSL_INCLUDE_DIR}"
    )
else()
    # Try to find GSL on non-Windows systems
    find_package(GSL REQUIRED)
endif()


# Make libamtrack available for linking
FetchContent_MakeAvailable(libamtrack)

# Fetch pybind11 for Python-C++ bindings
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.13.6  # Stable version for compatibility
)
FetchContent_MakeAvailable(pybind11)

# Locate Python3 development environment
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Set RPATH to the directory where libamtrack.so is installed
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_RPATH "$ORIGIN/../lib")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Ensure RPATH is not removed
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# Build the pyamtrack Python extension module
pybind11_add_module(pyamtrack MODULE
    src/pyamtrack.cpp  # Main binding source file
)

# Ensure libamtrack is built before pyamtrack
add_dependencies(pyamtrack amtrack)

# Link against libamtrack and GSL libraries
target_link_libraries(pyamtrack PRIVATE amtrack GSL::gsl GSL::gslcblas)

# Include libamtrack and GSL header directories
target_include_directories(pyamtrack PRIVATE
    ${libamtrack_SOURCE_DIR}/include
)

# Define export symbols for Windows shared libraries
if (WIN32)
    target_compile_definitions(pyamtrack PRIVATE -DAMTRACK_EXPORTS)
endif()

# Set installation paths and rules for Python packaging
include(GNUInstallDirs)

install(TARGETS pyamtrack
   EXPORT pyamtrackTargets
   LIBRARY DESTINATION pyamtrack
)

# Generate and install package configuration files
include(CMakePackageConfigHelpers)

# Set RPATH to allow finding libraries from the installation path
#set(CMAKE_INSTALL_RPATH "$ORIGIN")
