#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "electron_range.h"

namespace nb = nanobind;

/**
 * @brief Python module bindings for electron range calculations.
 *
 * This module exposes the electron range calculation functionality to Python.
 * It defines the module interface, submodule for models, and all public functions.
 */
NB_MODULE(electron_range, m) {
  m.doc() = R"pbdoc(
    Calculate maximum electron (delta ray) range in materials.

    This module provides functions to calculate the maximum distance that secondary electrons
    (delta rays) can travel in a material before losing all their energy. This is essential for:
    - Track structure calculations
    - Radial dose distribution (RDD) modeling
    - Geiss/Scholz local effect integration (LEI)
    - Understanding secondary particle effects in ion therapy

    The module supports multiple empirical and theoretical models for electron range calculations.
  )pbdoc";

  // ============================================================================
  // Submodule: models
  // ============================================================================
  nb::module_ models = m.def_submodule("models", "Available electron range calculation models");

  // Add model constants to the submodule
  // Users can access these as: pyamtrack.electron_range.models.tabata
  for (const auto& [name, id] : ELECTRON_RANGE_MODELS) {
    models.attr(name.c_str()) = nb::int_(id);
  }

  // ============================================================================
  // Functions
  // ============================================================================

  /**
   * @brief Get list of available model names.
   *
   * Returns all model names that can be passed to max_range() as the 'model' parameter.
   */
  m.def("get_models", &get_models,
        R"pbdoc(
        Get list of available electron range models.

        Returns
        -------
        list[str]
            List of model names that can be used as the `model` argument in max_range().

        Examples
        --------
        >>> import pyamtrack
        >>> pyamtrack.electron_range.get_models()
        ['butts_katz', 'waligorski', 'geiss', 'scholz', 'edmund', 'tabata', 'scholz_new']
      )pbdoc");

  /**
   * @brief Convert model name to model ID.
   *
   * This is mainly for internal use and debugging. Most users will pass model names
   * directly to max_range().
   */
  m.def("model", &get_model_id, nb::arg("name"),
        R"pbdoc(
        Convert model name to model ID.

        Parameters
        ----------
        name : str
            The model name (e.g., 'tabata', 'scholz').

        Returns
        -------
        int
            The numerical model ID used internally by libamtrack.

        Raises
        ------
        ValueError
            If the model name is not recognized.

        Examples
        --------
        >>> import pyamtrack
        >>> pyamtrack.electron_range.model("tabata")
        7
      )pbdoc");

  /**
   * @brief Calculate maximum electron range in a material.
   *
   * This is the main function in the module. It computes the maximum distance
   * that secondary electrons can travel in a given material at a given energy.
   */
  m.def("max_range", &max_range, nb::arg("energy_MeV"), nb::arg("material") = nb::int_(1),
        nb::arg("model") = nb::str("tabata"), nb::arg("cartesian_product") = false,
        R"pbdoc(
        Calculate maximum electron range in a material.

        Computes the maximum distance that secondary electrons (delta rays) can travel
        in a material before losing all their energy. This represents the track radius
        around the ion path and is used for track structure models.

        Parameters
        ----------
        energy_MeV : float, list, or numpy.ndarray
            The electron kinetic energy in MeV. Can be a single value, a Python list,
            or a NumPy array of any shape.
        material : int, Material, list, or numpy.ndarray, optional
            Either a material ID as an integer, a Material object, or an array/list of
            materials. Defaults to 1 (Liquid water).
        model : str, int, list, or numpy.ndarray, optional
            The electron range model to use. Can be specified as:
            - A string name: 'butts_katz', 'waligorski', 'geiss', 'scholz', 'edmund',
                            'tabata' (default), or 'scholz_new'
            - An integer model ID (2-8)
            - A list or array of models
        cartesian_product : bool, optional
            If True, compute the cartesian product over all arguments (all combinations).
            If False (default), apply element-wise operations with broadcasting.

        Returns
        -------
        float or numpy.ndarray
            The calculated electron range(s) in meters. The return type matches the input:
            - Single float for scalar energy input
            - NumPy array for array/list energy input
            - Nested arrays for cartesian product with multiple arguments

        Raises
        ------
        TypeError
            If material is neither an integer nor a Material object,
            or if model is neither a string nor an integer.
        ValueError
            If the model name is not recognized or model/material ID is invalid.

        Notes
        -----
        The electron range is used to compute:
        - Track structure (radial dose distribution around the ion path)
        - Local effect integration (LEI) for cell survival
        - Secondary particle effects in radiotherapy dose calculations

        Different models represent different empirical fits to experimental data.
        The default model (Tabata) is widely used and validated.

        Examples
        --------
        Single energy calculation in water (default material):

        >>> import pyamtrack
        >>> r = pyamtrack.electron_range.max_range(1.0)  # 1 MeV electron
        >>> print(f"Range: {r*1000:.3f} mm")
        Range: 0.670 mm

        Array of energies:

        >>> import numpy as np
        >>> energies = np.array([0.5, 1.0, 2.0, 5.0])
        >>> ranges = pyamtrack.electron_range.max_range(energies)
        >>> print(ranges)
        [0.000267 0.000670 0.001869 0.006788]

        Different material (Aluminum, ID=3):

        >>> r_al = pyamtrack.electron_range.max_range(1.0, material=3)

        Different model (Scholz instead of default Tabata):

        >>> r_scholz = pyamtrack.electron_range.max_range(1.0, model="scholz")

        Using Material object:

        >>> from pyamtrack.materials import Material
        >>> water = Material(1)
        >>> r = pyamtrack.electron_range.max_range(1.0, material=water)

        Cartesian product (all combinations):

        >>> energies = [0.5, 1.0, 2.0]
        >>> materials = [1, 3]  # water, aluminum
        >>> ranges = pyamtrack.electron_range.max_range(energies, material=materials, cartesian_product=True)
        >>> print(ranges.shape)
        (3, 2)  # 3 energies × 2 materials
      )pbdoc");
}
