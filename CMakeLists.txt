# Specify the minimum CMake version and compatibility range.
cmake_minimum_required(VERSION 3.15...3.29)

# Define the project using variables provided by SKBUILD.
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES C CXX
)

# Warn if the user invokes CMake directly
if (NOT SKBUILD)
  message(WARNING "\
  This CMake file is meant to be executed using 'scikit-build-core'.
  Running it directly will almost certainly not produce the desired
  result. If you are a user trying to install this package, use the
  command below, which will install all necessary build dependencies,
  compile the package in an isolated environment, and then install it.
  =====================================================================
   $ pip install .
  =====================================================================
  If you are a software developer, and this is your own package, then
  it is usually much more efficient to install the build dependencies
  in your environment once and use the following command that avoids
  a costly creation of a new virtual environment at every compilation:
  =====================================================================
   $ pip install nanobind scikit-build-core[pyproject]
   $ pip install --no-build-isolation -ve .
  =====================================================================
  You may optionally add -Ceditable.rebuild=true to auto-rebuild when
  the package is imported. Otherwise, you need to rerun the above
  after editing C++ files.")
endif()

if (CMAKE_VERSION VERSION_LESS 3.18)
  set(DEV_MODULE Development)
else()
  set(DEV_MODULE Development.Module)
endif()

###############################################################################
# Set C++17 as the required standard
###############################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###############################################################################
# Locate Python and nanobind
###############################################################################
# Find Python interpreter, development headers, and library.
find_package(Python 3.8 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Detect the installed nanobind package and import it into CMake
execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

###############################################################################
# Configure RPATH for non-Windows platforms
###############################################################################
if(NOT WIN32)
  # Set the install RPATH to the directory containing the executable/library.
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

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

  # Normalize paths to use forward slashes for compatibility.
  file(TO_CMAKE_PATH "${GSL_DLL}" GSL_DLL)
  file(TO_CMAKE_PATH "${GSL_CBLAS_DLL}" GSL_CBLAS_DLL)

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
# Fetch and configure libamtrack from GitHub
###############################################################################
# Fetch the libamtrack library from its GitHub repository.
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

# Make libamtrack available for the project.
FetchContent_MakeAvailable(libamtrack)

# Link libamtrack against GSL libraries.
target_link_libraries(amtrack PRIVATE GSL::gsl GSL::gslcblas)

###############################################################################
# Build the Python module (_core) and other targets
###############################################################################
# Add dynamic lookup for macOS to resolve Python symbols at runtime.
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
endif()

# Create the Python module from the source file
nanobind_add_module(_core src/main.cpp)

# Define a list of targets to link against the required libraries.
set(PYAMTRACK_TARGETS converters stopping materials)

foreach(TARGET ${PYAMTRACK_TARGETS})
  # Create a library for each target.
  file(GLOB SOURCES src/${TARGET}/*.cpp)
  nanobind_add_module(${TARGET} ${SOURCES})
endforeach()

# Loop through the targets and link them against the required libraries.
foreach(TARGET ${PYAMTRACK_TARGETS} _core)
  target_link_libraries(${TARGET} PRIVATE
    amtrack
    GSL::gsl
    GSL::gslcblas
  )
endforeach()

# Pass the project version as a preprocessor definition.
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

###############################################################################
# Installation configuration
###############################################################################
message(STATUS "Project version: ${PROJECT_VERSION}")

# Install the _core module into the pyamtrack directory.
install(TARGETS _core DESTINATION pyamtrack)

# Install the libamtrack shared library (amtrack.dll) into the same package directory.
install(TARGETS amtrack ${PYAMTRACK_TARGETS}
  LIBRARY DESTINATION pyamtrack
  RUNTIME DESTINATION pyamtrack
  ARCHIVE DESTINATION pyamtrack
)

# Install the GSL DLLs on Windows so they are present in the package folder.
if(WIN32)
  install(FILES "${GSL_DLL}" "${GSL_CBLAS_DLL}" DESTINATION pyamtrack)
endif()

###############################################################################
# Configure RPATH for installed targets
###############################################################################
if(WIN32)
  # RPATH is not used on Windows.
elseif(APPLE)
  set_target_properties(_core ${PYAMTRACK_TARGETS} PROPERTIES INSTALL_RPATH "@loader_path")
else()
  set_target_properties(_core ${PYAMTRACK_TARGETS} PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()
