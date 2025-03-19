cmake_minimum_required(VERSION 3.16)
project(pyamtrack VERSION 0.1.0 LANGUAGES CXX C)

# Enable C++17 or later
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable position-independent code for shared library compilation
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Use vcpkg toolchain if available
if (DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()


# Fetch libamtrack
include(FetchContent)
FetchContent_Declare(
    libamtrack
    GIT_REPOSITORY https://github.com/libamtrack/library.git
    GIT_TAG master
)

# Set options before calling FetchContent_MakeAvailable
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(libamtrack)

# Fetch pybind11
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.13.6  # Ensure this is an appropriate stable version
)
FetchContent_MakeAvailable(pybind11)

# Find dependencies via vcpkg
find_package(GSL REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Define Python extension module
pybind11_add_module(pyamtrack MODULE
    src/pyamtrack.cpp  # Main wrapper source file
)

# Ensure libamtrack is built before pyamtrack
add_dependencies(pyamtrack amtrack)

# Link against libamtrack, GSL
target_link_libraries(pyamtrack PRIVATE amtrack GSL::gsl GSL::gslcblas)

# Include directories
target_include_directories(pyamtrack PRIVATE
    ${libamtrack_SOURCE_DIR}/include  # Ensure libamtrack headers are available
)

# Ensure Windows DLL exports symbols correctly
if (WIN32)
    target_compile_definitions(pyamtrack PRIVATE -DAMTRACK_EXPORTS)
endif()


# Define installation rules for Python packaging
include(GNUInstallDirs)

# Ensure module is installed in the correct site-packages directory
#install(TARGETS pyamtrack
#    EXPORT pyamtrackTargets  # Ensure this export is defined
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
#)

install(TARGETS pyamtrack
    EXPORT pyamtrackTargets  # Ensure this export is defined
    LIBRARY DESTINATION ${Python3_SITEARCH}
)


# Configure CMake export settings
install(EXPORT pyamtrackTargets
    FILE pyamtrackTargets.cmake
    NAMESPACE pyamtrack::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pyamtrack
)

# Add CMake package configuration
include(CMakePackageConfigHelpers)
# Define the package version
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pyamtrackConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}  # This fixes the error
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pyamtrackConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pyamtrackConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pyamtrack
)


install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pyamtrackConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/pyamtrackConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pyamtrack
)

# Create a package for Python (wheel)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Set RPATH for proper linkage
set(CMAKE_INSTALL_RPATH "$ORIGIN")
