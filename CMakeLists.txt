# Specify the minimum CMake version and compatibility range.
cmake_minimum_required(VERSION 3.15...3.29)

# Define the project using variables provided by SKBUILD.
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES C CXX
)

###############################################################################
# Set C++17 as the required standard
###############################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###############################################################################
# Locate Python and pybind11
###############################################################################
# Find Python interpreter, development headers, and library.
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# Find pybind11 for Python bindings.
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

# Create the Python module from the source file.
python_add_library(_core MODULE src/main.cpp WITH_SOABI)

# Define a list of targets to link against the required libraries.
set(PYAMTRACK_TARGETS converters stopping materials)

foreach(TARGET ${PYAMTRACK_TARGETS})
  # Create a library for each target.
  file(GLOB SOURCES src/${TARGET}/*.cpp)
  python_add_library(${TARGET} MODULE ${SOURCES} WITH_SOABI)
endforeach()

# Ensure that _core is built after libamtrack.
add_dependencies(_core amtrack)

# Loop through the targets and link them against the required libraries.
foreach(TARGET ${PYAMTRACK_TARGETS} _core)
  target_link_libraries(${TARGET} PRIVATE
    pybind11::headers
    amtrack
    GSL::gsl
    GSL::gslcblas
  )
endforeach()

# Add this block to explicitly link Python on Windows.
if(WIN32)
  # Ensure Python library is linked.
  foreach(TARGET ${PYAMTRACK_TARGETS} _core)
    target_link_libraries(${TARGET} PRIVATE ${Python_LIBRARIES})
  endforeach()  
endif()

# Include Python headers for all targets.
foreach(TARGET ${PYAMTRACK_TARGETS} _core)
  target_include_directories(${TARGET} PRIVATE ${Python_INCLUDE_DIRS})
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