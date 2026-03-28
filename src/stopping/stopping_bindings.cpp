#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "stopping_power.h"
#include "electron_range.h"
#include "csda_range.h"

namespace nb = nanobind;

NB_MODULE(stopping, m) {
  m.doc() =
      "Functions for calculating stopping power of ions and protons and range of particles in "
      "materials.";

  // Create submodule for models
  nb::module_ models = m.def_submodule("models", "Stopping power models");

  // Add model constants using the map
  for (const auto& [name, id] : STOPPING_MODELS) {
    models.attr(name.c_str()) = nb::int_(id);
  }

  m.def("get_models", &get_models, "Returns list of available stopping power models");
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
            Either a material ID as integer or a Material object. Defaults to 1 (Liquid water).
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
            The calculated electron range(s) in meters. Returns a float for a single input,
            a NumPy array for a NumPy array input, a Python list for a list input and a NumPy array
            when computing a cartesian product.

        Raises
        ------
        TypeError
            If material argument is neither an integer nor a Material object,
            or if model argument is neither a string nor an integer.
        ValueError
            If the input energy is negative or the model/material ID is invalid.
        )pbdoc");
  m.def("mass_stopping_power", &mass_stopping_power,
      nb::arg("E_MeV_u"),
      nb::arg("particle") = 1001,
      nb::arg("material") = 1,
      nb::arg("source") = 2,
      nb::arg("cartesian_product") = false,
      R"pbdoc(
      Calculate mass stopping power in MeV*cm2/g.

      Parameters
      ----------
      E_MeV_u : float or array_like
          Kinetic energy in MeV per nucleon.
      particle : int or Particle, optional
          Particle number (1000*Z + A) or Particle object. Defaults to 1001 (proton).
      material : int or Material, optional
          Material ID or Material object. Defaults to 1 (Liquid water).
      source : int, optional
          Stopping power data source. 1=Bethe, 2=PSTAR (default), 3=ICRU.
      cartesian_product : bool, optional
          Whether to compute cartesian product over arguments.

      Returns
      -------
      float or numpy.ndarray
          Mass stopping power in MeV*cm2/g.
      )pbdoc");

  m.def("stopping_power", &stopping_power,
      nb::arg("E_MeV_u"),
      nb::arg("particle") = 1001,
      nb::arg("material") = 1,
      nb::arg("source") = 2,
      nb::arg("cartesian_product") = false,
      R"pbdoc(
      Calculate stopping power in keV/um.

      Parameters
      ----------
      E_MeV_u : float or array_like
          Kinetic energy in MeV per nucleon.
      particle : int or Particle, optional
          Particle number (1000*Z + A) or Particle object. Defaults to 1001 (proton).
      material : int or Material, optional
          Material ID or Material object. Defaults to 1 (Liquid water).
      source : int, optional
          Stopping power data source. 1=Bethe, 2=PSTAR (default), 3=ICRU.
      cartesian_product : bool, optional
          Whether to compute cartesian product over arguments.

      Returns
      -------
      float or numpy.ndarray
          Stopping power in keV/um.
      )pbdoc");
  m.def("debug_msp", []() { return 123; });

  m.def("csda_range", &csda_range,
      nb::arg("E_MeV_u"),
      nb::arg("particle") = 1001,
      nb::arg("material") = 1,
      nb::arg("cartesian_product") = false,
      R"pbdoc(
      Calculate CSDA range in g/cm^2.

      Calculates the range from initial energy to complete energy loss (E_final=0).

      Parameters
      ----------
      E_MeV_u : float or array_like
          Kinetic energy in MeV per nucleon.
      particle : int or Particle, optional
          Particle number (1000*Z + A) or Particle object. Defaults to 1001 (proton).
      material : int or Material, optional
          Material ID or Material object. Defaults to 1 (Liquid water).
      cartesian_product : bool, optional
          Whether to compute cartesian product over arguments.

      Returns
      -------
      float or numpy.ndarray
          CSDA range in g/cm^2.
      )pbdoc");
}
