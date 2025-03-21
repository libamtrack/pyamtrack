cmake_minimum_required(VERSION 3.15...3.29)
project(pyamtrack VERSION 0.1.0 LANGUAGES CXX C)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

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

pybind11_add_module(pyamtrack src/pyamtrack.cpp)

# Ensure libamtrack is built before pyamtrack
add_dependencies(pyamtrack amtrack)

target_link_libraries(pyamtrack PRIVATE amtrack GSL::gsl GSL::gslcblas)

# Include libamtrack and GSL header directories
target_include_directories(pyamtrack PRIVATE
    ${libamtrack_SOURCE_DIR}/include
)

install(TARGETS pyamtrack DESTINATION .)