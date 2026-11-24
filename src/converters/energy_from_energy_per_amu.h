#ifndef ENERGY_FROM_ENERGY_PER_AMU_H
#define ENERGY_FROM_ENERGY_PER_AMU_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object energy_from_energy_per_amu(nb::object energy_MeV_u, nb::object particle_no, bool cartesian_product);

#endif  // ENERGY_FROM_ENERGY_PER_AMU_H
