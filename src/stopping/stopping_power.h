#ifndef STOPPING_POWER_H
#define STOPPING_POWER_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "../materials/materials.h"

namespace nb = nanobind;

// Stopping power source IDs (matching enum in AT_StoppingPowerData.h)
// For now just PSTAR, extend later
const int SOURCE_PSTAR = 2;

/**
 * @brief Calculate mass stopping power in MeV*cm2/g.
 *
 * Wraps AT_Mass_Stopping_Power_with_no from libamtrack.
 *
 * @param E_MeV_u    Kinetic energy in MeV per nucleon. Scalar, list, or numpy array.
 * @param particle    Particle number (1000*Z + A) or Particle object. Default: 1001 (proton).
 * @param material    Material ID or Material object. Default: 1 (liquid water).
 * @param source      Stopping power source ID. Default: 2 (PSTAR).
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @return            Mass stopping power in MeV*cm2/g.
 */
nb::object mass_stopping_power(const nb::object& E_MeV_u,
                               const nb::object& particle = nb::int_(1001),
                               const nb::object& material = nb::int_(1),
                               int source = SOURCE_PSTAR,
                               bool cartesian_product = false);

/**
 * @brief Calculate stopping power in keV/um.
 *
 * Wraps AT_Stopping_Power_with_no from libamtrack.
 *
 * @param E_MeV_u    Kinetic energy in MeV per nucleon. Scalar, list, or numpy array.
 * @param particle    Particle number (1000*Z + A) or Particle object. Default: 1001 (proton).
 * @param material    Material ID or Material object. Default: 1 (liquid water).
 * @param source      Stopping power source ID. Default: 2 (PSTAR).
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @return            Stopping power in keV/um.
 */
nb::object stopping_power(const nb::object& E_MeV_u,
                               const nb::object& particle = nb::int_(1001),
                               const nb::object& material = nb::int_(1),
                               int source = SOURCE_PSTAR,
                               bool cartesian_product = false);

/**
 * @brief Calculate CSDA range in g/cm^2.
 *
 * Wraps AT_CSDA_range_g_cm2_single from libamtrack.
 * Calculates the range from E_initial to E_final=0 (complete energy loss).
 *
 * @param E_MeV_u    Kinetic energy in MeV per nucleon. Scalar, list, or numpy array.
 * @param particle    Particle number (1000*Z + A) or Particle object. Default: 1001 (proton).
 * @param material    Material ID or Material object. Default: 1 (liquid water).
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @return            CSDA range in g/cm^2.
 */
nb::object csda_range(const nb::object& E_MeV_u,
                         const nb::object& particle = nb::int_(1001),
                         const nb::object& material = nb::int_(1),
                         bool cartesian_product = false);


#endif  // STOPPING_POWER_H