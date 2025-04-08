#ifndef ELECTRON_RANGE_H
#define ELECTRON_RANGE_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

py::object PYBIND11_EXPORT electron_range(py::object input, py::object material = py::int_(1));

#endif // ELECTRON_RANGE_H