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

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/pyamtrack)
message(STATUS "GSL will be installed to ${CMAKE_INSTALL_PREFIX}")
set(LIBAMTRACK_INSTALL OFF CACHE BOOL "Disable amtrack installation" FORCE)

include(FetchContent)
FetchContent_Declare(
  libamtrack
  GIT_REPOSITORY https://github.com/libamtrack/library.git
  GIT_TAG master
)

# Disable building examples and installation of libamtrack.
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")

# Ensure that libamtrack is built as shared libraries.
set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

###############################################################################
# Configure GSL dependency
###############################################################################



# include(ExternalProject)

# # Define the version of GSL you want to use
# set(GSL_VERSION 2.7.1)
# set(GSL_PREFIX ${CMAKE_BINARY_DIR}/gsl)

# # Download and build GSL as shared libraries
# ExternalProject_Add(
#     gsl
#     URL https://ftp.gnu.org/gnu/gsl/gsl-${GSL_VERSION}.tar.gz
#     PREFIX ${GSL_PREFIX}
#     CONFIGURE_COMMAND ${GSL_PREFIX}/src/gsl/configure --prefix=${GSL_PREFIX} --enable-shared
#     BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j
#     INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
#     BUILD_IN_SOURCE 1
# )

# # Ensure that GSL is installed before including its directories
# ExternalProject_Get_Property(gsl install_dir)
# set(GSL_INCLUDE_DIR ${install_dir}/include)
# set(GSL_LIBRARY_DIR ${install_dir}/lib)

# # Create custom CMake targets for GSL
# add_library(GSL::gsl SHARED IMPORTED)
# set_target_properties(GSL::gsl PROPERTIES
#     IMPORTED_LOCATION ${GSL_LIBRARY_DIR}/libgsl${CMAKE_SHARED_LIBRARY_SUFFIX}
#     # INTERFACE_INCLUDE_DIRECTORIES ${GSL_INCLUDE_DIR}
# )

# add_library(GSL::gslcblas SHARED IMPORTED)
# set_target_properties(GSL::gslcblas PROPERTIES
#     IMPORTED_LOCATION ${GSL_LIBRARY_DIR}/libgslcblas${CMAKE_SHARED_LIBRARY_SUFFIX}
#     # INTERFACE_INCLUDE_DIRECTORIES ${GSL_INCLUDE_DIR}
# )

# FetchContent_Declare(
#   gsl
#   GIT_REPOSITORY https://github.com/ampl/gsl.git
#   GIT_TAG v2.7.0
# )

# # Set GSL installation directories

# set(GSL_BUILD_TESTS OFF CACHE BOOL "Disable GSL tests" FORCE)
# set(GSL_BUILD_DOCS OFF CACHE BOOL "Disable GSL documentation" FORCE)
# set(NO_AMPL_BINDINGS 1 CACHE BOOL "Disable AMPL bindings" FORCE)
# set(GSL_DISABLE_TESTS 1 CACHE BOOL "Disable GSL tests" FORCE)
# set(DOCUMENTATION OFF CACHE BOOL "Disable GSL documentation" FORCE)
# set(GSL_INSTALL_MULTI_CONFIG ON CACHE BOOL "Enable multi-config installation" FORCE)
# set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
# set(MSVC_RUNTIME_DYNAMIC ON CACHE BOOL "Use dynamic MSVC runtime" FORCE)

# FetchContent_MakeAvailable(gsl)

# add_library(GSL::gsl ALIAS gsl)
# add_library(GSL::gslcblas ALIAS gsl)

###############################################################################
# Build libamtrack and the Python module (_core)
###############################################################################
FetchContent_MakeAvailable(libamtrack)

python_add_library(_core MODULE src/main.cpp WITH_SOABI)

# add_dependencies(amtrack GSL::gsl GSL::gslcblas)

add_dependencies(_core amtrack)

target_link_libraries(_core PRIVATE
  pybind11::headers
  amtrack
  # GSL::gsl
  # GSL::gslcblas
)

target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

###############################################################################
# Installation configuration
###############################################################################
install(TARGETS _core DESTINATION pyamtrack)
# install(TARGETS gsl gslcblas
#   LIBRARY DESTINATION pyamtrack
#   ARCHIVE DESTINATION pyamtrack
#   RUNTIME DESTINATION pyamtrack
#   INCLUDES DESTINATION pyamtrack/include
# )

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
