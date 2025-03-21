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
  # Use VCPKG for GSL on Windows.
  if(NOT DEFINED ENV{VCPKG_INSTALLED_DIR})
    message(FATAL_ERROR "VCPKG_INSTALLED_DIR environment variable is not set.")
  endif()

  set(GSL_INCLUDE_DIR "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/include")
  set(GSL_LIBRARY "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/lib/gsl.lib")
  set(GSL_CBLAS_LIBRARY "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/lib/gslcblas.lib")
  set(GSL_DLL "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/bin/gsl.dll")
  set(GSL_CBLAS_DLL "$ENV{VCPKG_INSTALLED_DIR}/x64-windows/bin/gslcblas.dll")

  # Create imported targets.
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
  # On macOS and Linux, use ExternalProject to compile GSL with shared libraries.
  include(ExternalProject)

  ExternalProject_Add(gsl
      URL https://ftp.gnu.org/gnu/gsl/gsl-2.8.tar.gz
      PREFIX ${CMAKE_BINARY_DIR}/gsl
      CONFIGURE_COMMAND ./configure --prefix=${CMAKE_BINARY_DIR}/gsl/install --enable-shared --disable-static
      BUILD_COMMAND make -j$(nproc)
      INSTALL_COMMAND make install
  )

  add_library(GSL::gsl SHARED IMPORTED)
  set_target_properties(GSL::gsl PROPERTIES
      IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/gsl/install/lib/libgsl.so
      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/gsl/install/include
  )

  add_library(GSL::gslcblas SHARED IMPORTED)
  set_target_properties(GSL::gslcblas PROPERTIES
      IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/gsl/install/lib/libgslcblas.so
      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/gsl/install/include
  )

  add_dependencies(GSL::gsl gsl)
  add_dependencies(GSL::gslcblas gsl)
endif()

###############################################################################
# Build libamtrack and the Python module (_core)
###############################################################################
FetchContent_MakeAvailable(libamtrack)

python_add_library(_core MODULE src/main.cpp WITH_SOABI)

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
  install(FILES "${GSL_DLL}" "${GSL_CBLAS_DLL}" DESTINATION pyamtrack)
else()
  install(TARGETS amtrack
    LIBRARY DESTINATION pyamtrack
    RUNTIME DESTINATION pyamtrack
    ARCHIVE DESTINATION pyamtrack
  )

  install(FILES
    ${CMAKE_BINARY_DIR}/gsl/install/lib/libgsl.so
    ${CMAKE_BINARY_DIR}/gsl/install/lib/libgslcblas.so
    DESTINATION pyamtrack
  )
endif()

if(WIN32)
  # No RPATH needed on Windows.
elseif(APPLE)
  set_target_properties(_core PROPERTIES INSTALL_RPATH "@loader_path")
else()
  set_target_properties(_core PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()
