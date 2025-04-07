#ifndef ENERGY_FROM_BETA_H
#define ENERGY_FROM_BETA_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

py::object PYBIND11_EXPORT energy_from_beta(py::object input);

#endif // ENERGY_FROM_BETA_H