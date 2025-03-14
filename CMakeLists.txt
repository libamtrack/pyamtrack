cmake_minimum_required(VERSION 3.16)
project(pyamtrack LANGUAGES CXX C)

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
    GIT_TAG 261-add-an-option-for-the-compilation-of-cmake-so-that-examples-may-be-excluded-from-build  # Change to a stable version/tag
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
# if (WIN32)
#     find_library(GETOPT_LIB NAMES getopt PATHS ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/lib)
#     find_path(GETOPT_INCLUDE_DIR getopt.h PATHS ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include)

#     if (NOT GETOPT_LIB)
#         message(FATAL_ERROR "getopt.lib not found! Make sure it's installed via vcpkg.")
#     endif()
# endif()
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Define Python extension module
pybind11_add_module(pyamtrack MODULE
    src/pyamtrack.cpp  # Main wrapper source file
)

# Ensure libamtrack is built before pyamtrack
#add_dependencies(pyamtrack libamtrack)

# Link against libamtrack, GSL, and getopt
target_link_libraries(pyamtrack PRIVATE amtrack GSL::gsl GSL::gslcblas)

# Include directories
target_include_directories(pyamtrack PRIVATE
    ${libamtrack_SOURCE_DIR}/include  # Ensure libamtrack headers are available
)

# Ensure Windows DLL exports symbols correctly
if (WIN32)
    target_compile_definitions(pyamtrack PRIVATE -DAMTRACK_EXPORTS)
endif()

# Install the module
install(TARGETS pyamtrack
    LIBRARY DESTINATION ${Python3_SITEARCH}
)
