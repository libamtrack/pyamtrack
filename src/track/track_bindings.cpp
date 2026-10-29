#include <nanobind/nanobind.h>

#include <list>
#include <string>

#include "track_radial.h"

namespace nb = nanobind;

namespace {

using RadialDoseFunction = nb::object (*)(const nb::object&, const nb::object&, const nb::object&, const nb::object&,
                                          const nb::object&, const nb::object&, const nb::object&, bool, bool);

const char* RADIAL_DOSE_COMMON_PARAMETERS = R"pbdoc(
          Parameters
          ----------
          radius_m : float or array_like
              Radial distance from the ion path in meters. A list or 1-D NumPy
              array forms the last axis of the result. Values must be finite
              and non-negative.
          energy_MeV_u : float or array_like
              Kinetic energy per nucleon in MeV/u. Must be finite, in
              (0, 1e6), and inside the range of the selected stopping-power
              table.
          particle : Ion, list[Ion], or numpy array of Ions
              The primary ion.
          material : int, Material, list[int | Material], or numpy int array
              Material ID or Material object. Default: 1 (liquid water).
)pbdoc";

const char* RADIAL_DOSE_COMMON_OPTIONS = R"pbdoc(          source : str or StoppingPowerSource, optional
              Stopping-power source: "default", "bethe", "pstar", or "icru".
              Default: "default".
          cartesian_product : bool, optional
              If True, combine energy_MeV_u, particle, and material as a
              Cartesian product. If False, evaluate them elementwise with
              scalar broadcasting. Default: False.
          allow_bethe_fallback : bool, optional
              Whether the "default" source may fall back to the Bethe formula
              when no tabulated data covers the material and energy.
              Default: False.

          Returns
          -------
          float or numpy.ndarray
              Dose in Gy. A float when radius_m and all configuration
              arguments are scalar; otherwise an array of shape
              (*configuration_shape, n_radii), where the radius axis is
              omitted for a scalar radius_m.
      )pbdoc";

void def_radial_dose(nb::module_& m, const char* name, RadialDoseFunction function,
                     const char* default_electron_range_model, const char* summary, const char* model_options) {
  // The composed docstrings must outlive the module definition.
  static std::list<std::string> docstrings;
  docstrings.push_back(std::string(summary) + RADIAL_DOSE_COMMON_PARAMETERS + model_options +
                       RADIAL_DOSE_COMMON_OPTIONS);

  m.def(name, function, nb::arg("radius_m"), nb::arg("energy_MeV_u"), nb::arg("particle"), nb::arg("material") = 1,
        nb::arg("rdd_parameters") = nb::none(), nb::arg("electron_range_model") = default_electron_range_model,
        nb::arg("source") = "default", nb::arg("cartesian_product") = false,
        nb::arg("allow_bethe_fallback") = false, docstrings.back().c_str());
}

}  // namespace

NB_MODULE(track, m) {
  m.doc() = "Functions for calculating track structure and radial dose distributions.";

  m.def("track_width", &track_width, nb::arg("energy_MeV"), nb::arg("material") = 1, nb::arg("model") = "tabata",
        nb::arg("cartesian_product") = false,
        R"pbdoc(
          Calculate the maximum delta-electron range (track width) in meters.

          Parameters
          ----------
          energy_MeV : float or array_like
              Kinetic energy per nucleon of the ion in MeV/u.
          material : int, Material, list[int | Material], or numpy int array
              The material ID or Material object. Default: 1 (liquid water).
          model : str, int, or ElectronRangeModel, optional
              The electron range model. Default: "tabata".
          cartesian_product : bool, optional
              If True, compute all combinations of iterable/array arguments.
              If False, compute elementwise. Default: False.

          Returns
          -------
          float or numpy.ndarray
              The maximum track width in meters.
      )pbdoc");

  def_radial_dose(m, "katz_point", &katz_point, "waligorski",
                  R"pbdoc(
          Katz point-target radial dose distribution in Gy.
)pbdoc",
                  R"pbdoc(          rdd_parameters : dict, optional
              Overrides of the defaults {"r_min_m": 1e-10, "d_min_Gy": 1e-10}.
              r_min_m (lower integration radius, m) must be positive;
              d_min_Gy (dose cut-off, Gy) must be non-negative.
          electron_range_model : str, int, or ElectronRangeModel, optional
              One of "butts_katz", "waligorski", or "edmund".
              Default: "waligorski".
)pbdoc");

  def_radial_dose(m, "geiss", &geiss, "geiss",
                  R"pbdoc(
          Geiss radial dose distribution in Gy [Geiss et al., 1998].
)pbdoc",
                  R"pbdoc(          rdd_parameters : dict, optional
              Overrides of the default {"a0_m": 5e-8}. a0_m (core radius, m)
              must be positive.
          electron_range_model : str, int, or ElectronRangeModel, optional
              Any electron range model. Default: "geiss".
)pbdoc");

  def_radial_dose(m, "katz_site", &katz_site, "waligorski",
                  R"pbdoc(
          Katz site radial dose distribution in Gy [Edmund et al., 2007],
          with a low-dose cut-off.
)pbdoc",
                  R"pbdoc(          rdd_parameters : dict, optional
              Overrides of the defaults {"a0_m": 5e-8, "d_min_Gy": 1e-10}.
              a0_m (site radius, m) must be positive; d_min_Gy (dose
              cut-off, Gy) must be non-negative.
          electron_range_model : str, int, or ElectronRangeModel, optional
              One of "butts_katz", "waligorski", or "edmund".
              Default: "waligorski".
)pbdoc");

  def_radial_dose(m, "cucinotta_point", &cucinotta_point, "tabata",
                  R"pbdoc(
          Cucinotta point-target radial dose distribution in Gy
          [Cucinotta et al., 1997].
)pbdoc",
                  R"pbdoc(          rdd_parameters : dict, optional
              Overrides of the defaults {"r_min_m": 5e-11, "d_min_Gy": 1e-10}.
              r_min_m (lower integration radius, m) must be positive;
              d_min_Gy (dose cut-off, Gy) must be non-negative.
          electron_range_model : str, int, or ElectronRangeModel, optional
              Any electron range model. Default: "tabata".
)pbdoc");

  def_radial_dose(m, "katz_extended_target", &katz_extended_target, "waligorski",
                  R"pbdoc(
          Katz point-target radial dose distribution averaged over an
          extended target, in Gy.
)pbdoc",
                  R"pbdoc(          rdd_parameters : dict, optional
              Overrides of the defaults
              {"r_min_m": 1e-10, "a0_m": 1e-8, "d_min_Gy": 1e-10}.
              r_min_m (lower integration radius of the point RDD, m) and
              a0_m (target radius, m) must be positive; d_min_Gy (dose
              cut-off, Gy) must be non-negative.
          electron_range_model : str, int, or ElectronRangeModel, optional
              One of "butts_katz", "waligorski", or "edmund".
              Default: "waligorski".
)pbdoc");

  def_radial_dose(m, "cucinotta_extended_target", &cucinotta_extended_target, "tabata",
                  R"pbdoc(
          Cucinotta point-target radial dose distribution averaged over an
          extended target, in Gy.
)pbdoc",
                  R"pbdoc(          rdd_parameters : dict, optional
              Overrides of the defaults
              {"r_min_m": 5e-11, "a0_m": 1e-8, "d_min_Gy": 1e-10}.
              r_min_m (lower integration radius of the point RDD, m) and
              a0_m (target radius, m) must be positive; d_min_Gy (dose
              cut-off, Gy) must be non-negative.
          electron_range_model : str, int, or ElectronRangeModel, optional
              Any electron range model. Default: "tabata".
)pbdoc");
}
