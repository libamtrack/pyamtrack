#ifndef STOPPING_POWER_H
#define STOPPING_POWER_H

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "../materials/materials.h"
#include "../particles/particle_arguments.h"
#include "stopping_power_source.h"

namespace nb = nanobind;

/**
 * @brief Calculate mass stopping power in MeV*cm2/g.
 *
 * Wraps AT_Mass_Stopping_Power_with_no from libamtrack.
 *
 * Accepts scalar and sequence inputs. By default, sequence inputs are evaluated
 * elementwise; setting cartesian_product enables Cartesian-product evaluation.
 *
 * @param energy_MeV_u  Kinetic energy in MeV per nucleon.
 * @param particle      Particle object.
 * @param material      Material ID or Material object.
 * @param source        StoppingPowerSource enum or case-insensitive name
 *                      ("default", "bethe", "pstar", "icru").
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @param allow_bethe_fallback  Whether to allow Bethe fallback when tabular data is requested but not available.
 * @param full_output  Whether to return a tuple of values and resolved source IDs.
 * @return              Mass stopping power in MeV*cm2/g.
 */
nb::object mass_stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                               const nb::object& source, bool cartesian_product, bool allow_bethe_fallback,
                               bool full_output);

/**
 * @brief Calculate stopping power in keV/um.
 *
 * Wraps AT_Stopping_Power_with_no from libamtrack.
 *
 * @param energy_MeV_u  Kinetic energy in MeV per nucleon.
 * @param particle      Particle object.
 * @param material      Material ID or Material object.
 * @param source        StoppingPowerSource enum or case-insensitive name
 *                      ("default", "bethe", "pstar", "icru").
 * @param cartesian_product  Whether to compute cartesian product over arguments.
 * @param allow_bethe_fallback  Whether to allow Bethe fallback when tabular data is requested but not available.
 * @param full_output  Whether to return a tuple of values and resolved source IDs.
 * @return              Stopping power in keV/um.
 */
nb::object stopping_power(const nb::object& energy_MeV_u, const nb::object& particle, const nb::object& material,
                          const nb::object& source, bool cartesian_product, bool allow_bethe_fallback,
                          bool full_output);

#endif  // STOPPING_POWER_H
