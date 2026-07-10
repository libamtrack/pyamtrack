#ifndef DOSE_BORTFELD_H
#define DOSE_BORTFELD_H

#include <nanobind/nanobind.h>

#include "../materials/materials.h"  // for process_material

namespace nb = nanobind;

/**
 * Compute proton dose at depth using Bortfeld analytical model.
 *
 * Supports scalar/list/numpy array inputs for z_cm (and optionally other args if you pass arrays),
 * and supports elementwise evaluation or cartesian-product evaluation.
 *
 * Parameters correspond to libamtrack:
 * - z_cm: depth [cm]
 * - fluence_cm2: fluence [1/cm^2]
 * - E_MeV: initial energy [MeV]
 * - sigma_E_MeV: energy spread [MeV] (negative => libamtrack default 0.01*E)
 * - material: int material_no OR pyamtrack.materials.Material (will be converted via process_material)
 * - eps: tail fraction, default 0.03
 * - cartesian_product: if true compute full cartesian product across iterable/array args
 */
nb::object dose_bortfeld(const nb::object& z_cm, const nb::object& fluence_cm2, const nb::object& E_MeV,
                         const nb::object& sigma_E_MeV, const nb::object& material = nb::int_(1),
                         const nb::object& eps = nb::float_(0.03), bool cartesian_product = false);

#endif  // DOSE_BORTFELD_H
