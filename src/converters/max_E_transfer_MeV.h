#ifndef VERSION_MAX_E_TRANSFER_MEV_H
#define VERSION_MAX_E_TRANSFER_MEV_H

#include <nanobind/nanobind.h>
#include <vector>
#include <functional>

namespace nb = nanobind;
nb::object max_E_transfer_MeV(nb::object E_MeV_u, nb::object A);

#endif //VERSION_MAX_E_TRANSFER_MEV_H