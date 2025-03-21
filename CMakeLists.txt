cmake_minimum_required(VERSION 3.15...3.29)
project(pyamtrack VERSION 0.1.0 LANGUAGES CXX C)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
    libamtrack
    GIT_REPOSITORY https://github.com/libamtrack/library.git
    GIT_TAG master
)

pybind11_add_module(pyamtrack src/pyamtrack.cpp)
install(TARGETS pyamtrack DESTINATION .)