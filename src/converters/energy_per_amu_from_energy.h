#ifndef ENERGY_PER_AMU_FROM_ENERGY_H
#define ENERGY_PER_AMU_FROM_ENERGY_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object energy_per_amu_from_energy(nb::object energy, nb::object particle_no, bool cartesian_product);

#endif  // ENERGY_PER_AMU_FROM_ENERGY_H
