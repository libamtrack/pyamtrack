#ifndef DOSE_BORTFELD_H
#define DOSE_BORTFELD_H

#include <nanobind/nanobind.h>

#include "../materials/materials.h"  // for validate_material_argument, parse_material_argument

namespace nb = nanobind;

/**
 * Compute proton dose at depth using Bortfeld analytical model.
 *
 * Supports scalar/list/numpy array inputs for z_cm (and optionally other args if you pass arrays),
 * and supports elementwise evaluation or cartesian-product evaluation.
 *
 * Parameters correspond to libamtrack:
 * - z_cm: depth [cm], must be >= 0
 * - fluence_cm2: fluence [1/cm^2]
 * - E_MeV: initial energy [MeV], must be in [0.1, 10000.0]
 * - sigma_E_fraction: energy spread as fraction of E_MeV (dimensionless), must be in (0, 1);
 *                     internally converted to sigma_E_MeV = sigma_E_fraction * E_MeV
 * - material: int material_no in [1, 24] OR pyamtrack.materials.Material object
 * - eps: nuclear tail fraction in [0, 1), default 0.03
 * - cartesian_product: if true compute full cartesian product across iterable/array args
 */
nb::object dose_bortfeld(const nb::object& z_cm, const nb::object& fluence_cm2, const nb::object& E_MeV,
                         const nb::object& sigma_E_fraction, const nb::object& material = nb::int_(1),
                         const nb::object& eps = nb::float_(0.03), bool cartesian_product = false);

#endif  // DOSE_BORTFELD_H
