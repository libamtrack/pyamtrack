#ifndef DOSE_BORTFELD_H
#define DOSE_BORTFELD_H

#include <nanobind/nanobind.h>

#include "../materials/materials.h"  // for validate_material_argument, parse_material_argument

namespace nb = nanobind;

/**
 * Compute proton dose at depth using Bortfeld analytical model.
 *
 * Accepts scalar and sequence inputs for z_cm and the other arguments. By default, sequence inputs
 * are evaluated elementwise; setting cartesian_product enables Cartesian-product evaluation.
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
nb::object dose_bortfeld(const nb::object& depth_cm, const nb::object& fluence_cm2, const nb::object& energy_MeV,
                         const nb::object& energy_spread_fraction, const nb::object& material,
                         const nb::object& eps, bool cartesian_product);

#endif  // DOSE_BORTFELD_H
