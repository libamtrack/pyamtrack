#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "electron_range.h"
#include "stopping_power.h"

namespace nb = nanobind;

NB_MODULE(stopping, m) {
  m.doc() =
      "Functions for calculating stopping power of ions and protons and range of particles in "
      "materials.";

  nb::enum_<StoppingPowerSource>(m, "StoppingPowerSource")
      .value("DEFAULT", StoppingPowerSource::Default)
      .value("BETHE", StoppingPowerSource::Bethe)
      .value("PSTAR", StoppingPowerSource::PSTAR)
      .value("ICRU", StoppingPowerSource::ICRU)
      .export_values();

  nb::module_ models = m.def_submodule("models", "Electron range models");
  for (const auto& [name, id] : STOPPING_MODELS) {
    models.attr(name.c_str()) = nb::int_(id);
  }

  m.def("get_models", &get_models, "Returns list of available electron range models");
  m.def("model", &get_model_id, nb::arg("name"), "Returns model ID for given model name");

  m.def("electron_range", &electron_range, nb::arg("energy_MeV"), nb::arg("material") = 1, nb::arg("model") = "tabata",
        nb::arg("cartesian_product") = false,
        R"pbdoc(
        Calculate electron range in meters using various models.

        This function calculates the maximum electron range in a material using different theoretical
        or empirical models. The range represents the maximum distance that electrons can travel
        in the material before losing all their energy.

        Parameters
        ----------
        energy_MeV : float or array_like
            The electron energy in MeV. Can be a single value, a NumPy array, or a Python list.
        material : int, Material, list[int | Material] or numpy array with int as dtype, optional
            Either a material ID as integer or a Material object. Boolean values are not accepted.
            Defaults to 1 (Liquid water).
        model : str, int, list[int | str] or numpy array with int as dtype, optional
            The stopping power model to use. Can be specified either as a string name or model ID.
            Available models:
            - "butts_katz" (id=2): Butts & Katz model
            - "waligorski" (id=3): Waligorski model
            - "geiss" (id=4): Geiss model
            - "scholz" (id=5): Scholz model
            - "edmund" (id=6): Edmund model
            - "tabata" (id=7): Tabata model (default)
            - "scholz_new" (id=8): Updated Scholz model
        cartesian_product: bool
            Indicates whether to compute cartesian product over passed arguments.

        Returns
        -------
        float or numpy.ndarray
            The calculated electron range(s) in meters. Returns a float when all inputs are scalar
            and a NumPy array when any input is a list or array, including when computing a
            cartesian product.

        Raises
        ------
        TypeError
            If material argument is neither an integer nor a Material object, is a bool,
            or if model argument is neither a string nor an integer.
        ValueError
            If the input energy is negative or the model/material ID is invalid.
        )pbdoc");
  m.def("mass_stopping_power", &mass_stopping_power, nb::arg("energy_MeV_u"), nb::arg("particle"),
        nb::arg("material") = 1, nb::arg("source") = 0, nb::arg("cartesian_product") = false,
        nb::arg("allow_bethe_fallback") = false, nb::arg("full_output") = false,
        R"pbdoc(
        Calculate mass stopping power in MeV·cm²/g.

        Wraps AT_Mass_Stopping_Power_with_no from libamtrack.

        Parameters
        ----------
        energy_MeV_u : float or array_like
            Kinetic energy in MeV per nucleon. Must be > 0.
            Can be a float, a Python list, or a NumPy array.
        particle : Ion, list[Ion].
            libamtrack particle number (1000*Z + A), or a pyamtrack.particles.ions.Ion
            object. Boolean values are not accepted. Elementary particles (neutron,
            electron) are not supported. Default: 1001 (proton).
        material : int, Material, list[int | Material], or numpy int array, optional
            Any material ID returned by pyamtrack.materials.get_ids(), or a
            pyamtrack.materials.Material object. Boolean values are not accepted.
            Default: 1 (liquid water).
        source : str or StoppingPowerSource, optional
            Stopping-power data source:
              - "default" : PSTAR when tabulated data cover the material and
                            energy (IDs 1-9), otherwise Bethe (case-insensitive).
              - "bethe"   : analytical Bethe formula (case-insensitive).
              - "pstar"   : NIST PSTAR tables (case-insensitive). Available for
                            material IDs 1-9.
              - "icru"    : ICRU 49/73 tables (case-insensitive). Available for
                            liquid water and aluminum oxide.
            The StoppingPowerSource.DEFAULT / BETHE / PSTAR / ICRU enum members
            are accepted as well. Default: "default".
        cartesian_product : bool, optional
            If True, compute all combinations of iterable/array arguments (cartesian product).
            If False, compute elementwise. Default: False.
        allow_bethe_fallback : bool, optional
            If True, allow fallback to the Bethe formula when the requested source has no data for the material.
            If False, all elements must resolve to only tabular data. Default: False.
        full_output : bool, optional
            If True, return ``(values, source_ids)``. The source IDs are the
            resolved libamtrack sources: Bethe=1, PSTAR=2, and ICRU=3.
            Default: False.

        Returns
        -------
        float or numpy.ndarray
            Mass stopping power in MeV·cm²/g.

        Raises
        ------
        TypeError
            If particle or material is not an int, object, list, or int numpy array,
            if either is a bool, or if source is not a string or StoppingPowerSource.
        ValueError
            If energy_MeV_u is <= 0, source is not a known name, a material or particle
            ID is invalid, the requested source has no data for the material, or
            energy_MeV_u is outside the selected source's tabulated range.
      )pbdoc");

  m.def("stopping_power", &stopping_power, nb::arg("energy_MeV_u"), nb::arg("particle"), nb::arg("material") = 1,
        nb::arg("source") = 0, nb::arg("cartesian_product") = false, nb::arg("allow_bethe_fallback") = false,
        nb::arg("full_output") = false,
        R"pbdoc(
        Calculate density-scaled stopping power in keV/µm.

        The arguments accept scalar and sequence values. By default, sequence
        values are evaluated elementwise; cartesian_product=True evaluates all
        combinations. If full_output=True, return (values, source_ids), where
        source_ids has the same scalar or array shape as values.
        )pbdoc");
}
