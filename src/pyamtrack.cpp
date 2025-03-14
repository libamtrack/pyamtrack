#include <pybind11/pybind11.h>
#include "AT_Constants.h"  // Example libamtrack header

namespace py = pybind11;

// Example wrapper function
double example_function(double x) {
    return x * 2.0;  // Replace with an actual libamtrack function
}

PYBIND11_MODULE(pymatrack, m) {
    m.doc() = "Python bindings for libamtrack";
    m.def("example_function", &example_function, "An example function");
}
