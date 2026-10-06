#ifndef FLUENCE_FROM_DOSE_H
#define FLUENCE_FROM_DOSE_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object fluence_from_dose(nb::object energy_MeV_u, nb::object particle, nb::object dose, nb::object material,
                             nb::object stopping_power_source, bool cartesian_product);
#endif  // FLUENCE_FROM_DOSE_H
