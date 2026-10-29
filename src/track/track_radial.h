#ifndef TRACK_RADIAL_H
#define TRACK_RADIAL_H

#include <nanobind/nanobind.h>

namespace nb = nanobind;

/**
 * @brief Calculate the maximum delta-electron range (track width) in meters.
 *
 * @param energy_MeV  Kinetic energy per nucleon.
 * @param material    Material ID or Material object (scalar or sequence).
 * @param model       Electron range model (ElectronRangeModel, name, or ID).
 */
nb::object track_width(const nb::object& energy_MeV, const nb::object& material, const nb::object& model,
                       bool cartesian_product);

/**
 * Model-specific radial dose distributions in Gy, wrapping AT_D_RDD_Gy.
 *
 * @param radius_m              Radial distance(s) from the ion path in meters; a sequence forms the last output axis.
 * @param energy_MeV_u          Kinetic energy per nucleon (scalar or sequence).
 * @param particle              Ion or sequence of Ions.
 * @param material              Material ID or Material object (scalar or sequence).
 * @param rdd_parameters        dict of model-specific parameters, or None for the libamtrack defaults.
 * @param electron_range_model  Electron range model compatible with the RDD model.
 * @param source                Stopping-power source.
 * @param cartesian_product     Whether energy/particle/material are combined as a Cartesian product.
 * @param allow_bethe_fallback  Whether the default source may fall back to the Bethe formula.
 */
#define DECLARE_RADIAL_DOSE_FUNCTION(function_name)                                                               \
  nb::object function_name(const nb::object& radius_m, const nb::object& energy_MeV_u, const nb::object& particle, \
                           const nb::object& material, const nb::object& rdd_parameters,                          \
                           const nb::object& electron_range_model, const nb::object& source,                      \
                           bool cartesian_product, bool allow_bethe_fallback);

DECLARE_RADIAL_DOSE_FUNCTION(katz_point)
DECLARE_RADIAL_DOSE_FUNCTION(geiss)
DECLARE_RADIAL_DOSE_FUNCTION(katz_site)
DECLARE_RADIAL_DOSE_FUNCTION(cucinotta_point)
DECLARE_RADIAL_DOSE_FUNCTION(katz_extended_target)
DECLARE_RADIAL_DOSE_FUNCTION(cucinotta_extended_target)

#undef DECLARE_RADIAL_DOSE_FUNCTION

#endif  // TRACK_RADIAL_H
