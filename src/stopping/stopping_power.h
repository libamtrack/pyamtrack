#ifndef STOPPING_POWER_H
#define STOPPING_POWER_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "../materials/materials.h"
#include "../particles/particle_arguments.h"

namespace nb = nanobind;

/**
 * Stopping-power data source, matching libamtrack ids for Bethe / PSTAR / ICRU.
 *
 * Default is resolved per material: PSTAR when tabulated data exist (IDs 1-9),
 * otherwise the analytical Bethe formula.
 */
enum class StoppingPowerSource { Default = 0, Bethe = 1, PSTAR = 2, ICRU = 3 };

/**
 * @brief Choose the default stopping-power source for a material.
 *
 * PSTAR tables exist for material IDs 1-9 (liquid water through copper).
 * Other materials fall back to the Bethe formula.
 */
StoppingPowerSource get_default_stopping_power_source(long material_id);

/**
 * @brief Calculate mass stopping power in MeV*cm2/g.
 *
 * Wraps AT_Mass_Stopping_Power_with_no from libamtrack.
 *
 * Accepts scalar and sequence inputs. By default, sequence inputs are evaluated
 * elementwise; setting cartesian_product enables Cartesian-product evaluation.
 *
 * @param energy_MeV_u  Kinetic energy in MeV per nucleon.
 * @param particle      Particle number (1000*Z + A) or Ion object.
 * @param material      Material ID or Material object.
 * @param source        StoppingPowerSource enum or case-insensitive name
 *                      ("default", "bethe", "pstar", "icru").
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @return              Mass stopping power in MeV*cm2/g.
 */
nb::object mass_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                               const nb::object& source, bool cartesian_product);

/**
 * @brief Calculate stopping power in keV/um.
 *
 * Wraps AT_Stopping_Power_with_no from libamtrack.
 *
 * @param energy_MeV_u  Kinetic energy in MeV per nucleon.
 * @param particle      Particle number (1000*Z + A) or Ion object.
 * @param material      Material ID or Material object.
 * @param source        StoppingPowerSource enum or case-insensitive name
 *                      ("default", "bethe", "pstar", "icru").
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @return              Stopping power in keV/um.
 */
nb::object stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                          const nb::object& source, bool cartesian_product);

#endif  // STOPPING_POWER_H
