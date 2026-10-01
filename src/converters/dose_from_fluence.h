#ifndef DOSE_FROM_FLUENCE_H
#define DOSE_FROM_FLUENCE_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

nb::object dose_from_fluence(nb::object energy_MeV_u,
                            nb::object particle,
                            nb::object fluence_cm2,
                            nb::object material,
                            nb::object stopping_power_source);
#endif  // DOSE_FROM_FLUENCE_H