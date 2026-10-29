#ifndef MOMENTUM_FROM_ENERGY_H
#define MOMENTUM_FROM_ENERGY_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object momentum_from_energy(nb::object energy_MeV, const nb::object particle, const bool cartesian_product);

#endif  // MOMENTUM_FROM_ENERGY_H