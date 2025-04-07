#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(materials, m) {
    m.doc() = "Materials";
}