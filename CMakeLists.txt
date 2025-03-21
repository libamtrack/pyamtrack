# Specify the minimum CMake version and compatibility range.
cmake_minimum_required(VERSION 3.15...3.29)

# Define the project using variables provided by SKBUILD.
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES C CXX
)

###############################################################################
# Locate Python and pybind11
###############################################################################
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

###############################################################################
# Configure RPATH for non-Windows platforms
###############################################################################
if(NOT WIN32)
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

###############################################################################
# Fetch and configure libamtrack from GitHub
###############################################################################
include(FetchContent)
FetchContent_Declare(
  libamtrack
  GIT_REPOSITORY https://github.com/libamtrack/library.git
  GIT_TAG 264-check-linking-to-cmake-gsl
)

# Disable building examples and installation of libamtrack.
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(LIBAMTRACK_INSTALL OFF CACHE BOOL "Disable amtrack installation" FORCE)

# Ensure that libamtrack is built as shared libraries.
set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

###############################################################################
# Configure GSL dependency
###############################################################################
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/ampl/gsl.git
  GIT_TAG v2.7.0
)

set(GSL_BUILD_TESTS OFF CACHE BOOL "Disable GSL tests" FORCE)
set(GSL_BUILD_DOCS OFF CACHE BOOL "Disable GSL documentation" FORCE)
set(NO_AMPL_BINDINGS 1 CACHE BOOL "Disable AMPL bindings" FORCE)
set(GSL_DISABLE_TESTS 1 CACHE BOOL "Disable GSL tests" FORCE)
set(DOCUMENTATION OFF CACHE BOOL "Disable GSL documentation" FORCE)

FetchContent_MakeAvailable(gsl)

add_library(GSL::gsl ALIAS gsl)
add_library(GSL::gslcblas ALIAS gsl)

###############################################################################
# Build libamtrack and the Python module (_core)
###############################################################################
FetchContent_MakeAvailable(libamtrack)

python_add_library(_core MODULE src/main.cpp WITH_SOABI)

add_dependencies(amtrack GSL::gsl GSL::gslcblas)

add_dependencies(_core amtrack)

target_link_libraries(_core PRIVATE
  pybind11::headers
  amtrack
  GSL::gsl
  GSL::gslcblas
)

target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

###############################################################################
# Installation configuration
###############################################################################
install(TARGETS _core DESTINATION pyamtrack)

if(WIN32)
  install(TARGETS amtrack RUNTIME DESTINATION pyamtrack)
else()
  install(TARGETS amtrack
    LIBRARY DESTINATION pyamtrack
    RUNTIME DESTINATION pyamtrack
    ARCHIVE DESTINATION pyamtrack
  )
endif()


if(WIN32)
  # No RPATH needed on Windows.
elseif(APPLE)
  set_target_properties(_core PROPERTIES INSTALL_RPATH "@loader_path")
else()
  set_target_properties(_core PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()
