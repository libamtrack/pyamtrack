#ifndef LET_WILKENS_H
#define LET_WILKENS_H

#include <nanobind/nanobind.h>

#include <string>

#include "../materials/materials.h"  // for validate_material_argument, parse_material_argument

namespace nb = nanobind;

enum class Averaging {
    Dose,
    Track
};

/**
 * Compute proton LET at depth using the Wilkens & Oelfke analytical model.
 *
 * Supports scalar/list/numpy array inputs for the numeric arguments, and supports
 * elementwise evaluation or cartesian-product evaluation.
 *
 * Parameters correspond to libamtrack:
 * - depth_cm: depth [cm], must be >= 0
 * - material: int material_no in [1, 24] OR pyamtrack.materials.Material object
 * - energy_MeV: initial energy [MeV], must be in [0.1, 10000.0]
 * - energy_spread_fraction: energy spread as fraction of energy_MeV (dimensionless), must be in (0, 1);
 *                           internally converted to sigma_E_MeV = energy_spread_fraction * energy_MeV
 * - averaging: "dose"/Averaging::Dose for LET_d, "track"/Averaging::Track for LET_t
 * - cartesian_product: if true compute full cartesian product across iterable/array args
 */
nb::object let_wilkens(const nb::object& depth_cm, const nb::object& material, const nb::object& energy_MeV,
                       const nb::object& energy_spread_fraction,
                       const nb::object& averaging = nb::cast(std::string("dose")),
                       bool cartesian_product = false);

#endif  // LET_WILKENS_H
