#ifndef GAMMA_FROM_ENERGY_H
#define GAMMA_FROM_ENERGY_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object gamma_from_energy(nb::object energy_MeV_u);

#endif  // GAMMA_FROM_ENERGY_H