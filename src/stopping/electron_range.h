#ifndef ELECTRON_RANGE_H
#define ELECTRON_RANGE_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

py::object PYBIND11_EXPORT electron_range(py::object input);

#endif // ELECTRON_RANGE_H