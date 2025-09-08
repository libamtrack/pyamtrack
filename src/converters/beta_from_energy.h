#ifndef BETA_FROM_ENERGY_H
#define BETA_FROM_ENERGY_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object beta_from_energy(nb::object energy_MeV_u);

#endif  // BETA_FROM_ENERGY_H
