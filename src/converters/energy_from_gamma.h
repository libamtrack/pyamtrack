#ifndef ENERGY_FROM_GAMMA_H
#define ENERGY_FROM_GAMMA_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object energy_from_gamma(nb::object gamma, const nb::object particle, const bool cartesian_product);

#endif  // ENERGY_FROM_GAMMA_H