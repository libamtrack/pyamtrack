#ifndef BETA_FROM_ENERGY_H
#define BETA_FROM_ENERGY_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

py::object PYBIND11_EXPORT beta_from_energy(py::object input);

#endif // BETA_FROM_ENERGY_H