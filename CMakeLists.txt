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
  # Set the install RPATH to the directory containing the executable/library.
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
  GIT_TAG master
)

# Disable building examples and installation of libamtrack.
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(LIBAMTRACK_INSTALL OFF CACHE BOOL "Disable amtrack installation" FORCE)

# Ensure that libamtrack is built as shared libraries.
set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

###############################################################################
# Configure GSL dependency
###############################################################################
if(WIN32)
  # On Windows, use VCPKG-installed GSL.
  if(NOT DEFINED ENV{VCPKG_INSTALLED_DIR})
    message(FATAL_ERROR "VCPKG_INSTALLED_DIR environment variable is not set.")
  endif()

  message(STATUS "VCPKG_INSTALLED_DIR (env variable): $ENV{VCPKG_INSTALLED_DIR}")

  # Define paths for GSL include and library files.
  set(GSL_INCLUDE_DIR "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/include")
  set(GSL_LIBRARY "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/lib/gsl.lib")
  set(GSL_CBLAS_LIBRARY "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/lib/gslcblas.lib")

  # Define the paths for the runtime DLLs (assuming they are in the bin folder).
  set(GSL_DLL "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/bin/gsl.dll")
  set(GSL_CBLAS_DLL "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/bin/gslcblas.dll")

  message(STATUS "Using GSL include directory: ${GSL_INCLUDE_DIR}")
  message(STATUS "Using GSL library: ${GSL_LIBRARY}")
  message(STATUS "Using GSL CBLAS library: ${GSL_CBLAS_LIBRARY}")
  message(STATUS "Using GSL DLL: ${GSL_DLL}")
  message(STATUS "Using GSL CBLAS DLL: ${GSL_CBLAS_DLL}")

  # Manually create imported targets for GSL libraries.
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
  # On non-Windows systems, try to locate GSL using find_package.
  find_package(GSL REQUIRED)
endif()

###############################################################################
# Build libamtrack and the Python module (_core)
###############################################################################
# Make libamtrack available.
FetchContent_MakeAvailable(libamtrack)

# Create the Python module from the source file.
python_add_library(_core MODULE src/main.cpp WITH_SOABI)

# Ensure that _core is built after libamtrack.
add_dependencies(_core amtrack)

# Link _core against required libraries.
target_link_libraries(_core PRIVATE
  pybind11::headers
  amtrack
  GSL::gsl
  GSL::gslcblas
)

# Pass the project version as a preprocessor definition.
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

###############################################################################
# Installation configuration
###############################################################################
# Install the _core module into the pyamtrack directory.
install(TARGETS _core DESTINATION pyamtrack)

# Install the libamtrack shared library (amtrack.dll) into the same package directory.
if(WIN32)
  install(TARGETS amtrack RUNTIME DESTINATION pyamtrack)
else()
  install(TARGETS amtrack
    LIBRARY DESTINATION pyamtrack
    RUNTIME DESTINATION pyamtrack
    ARCHIVE DESTINATION pyamtrack
  )
endif()

# Install the GSL DLLs on Windows so they are present in the package folder.
if(WIN32)
  install(FILES "${GSL_DLL}" "${GSL_CBLAS_DLL}" DESTINATION pyamtrack)
endif()

# Set the proper RPATH for the _core target based on the platform.
if(WIN32)
  # RPATH is not used on Windows.
elseif(APPLE)
  set_target_properties(_core PROPERTIES INSTALL_RPATH "@loader_path")
else()
  set_target_properties(_core PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()