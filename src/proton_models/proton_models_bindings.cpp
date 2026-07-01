#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>


#include "dose_bortfeld.h"
#include "let_wilkens.h"

namespace nb = nanobind;

NB_MODULE(proton_models, m) {
  m.doc() = "Analytical proton beam models (e.g., Bragg curve approximations, LET, RBE).";

  m.def("dose_bortfeld", &dose_bortfeld, nb::arg("z_cm"), nb::arg("fluence_cm2"), nb::arg("E_MeV"),
        nb::arg("sigma_E_MeV"), nb::arg("material") = 1, nb::arg("eps") = -1.0, nb::arg("cartesian_product") = false,
        R"pbdoc(
        Compute dose at depth for proton beams using the analytical model of Bortfeld (1997).

        Parameters
        ----------
        z_cm : float or array_like
            Depth in medium [cm]. Can be a float, a Python list, or a NumPy array.
        fluence_cm2 : float or array_like
            Proton fluence [1/cm^2]. Can be a float, a Python list, or a NumPy array.
        E_MeV : float or array_like
            Initial kinetic energy [MeV]. Can be a float, a Python list, or a NumPy array.
        sigma_E_MeV : float or array_like
            Kinetic energy spread (standard deviation) [MeV]. If negative, libamtrack uses
            the default value 0.01 * E_MeV.
        material : int, Material, list[int | Material] or numpy array with int as dtype, optional
            Medium material specification. Accepts:
              - material ID (int), or
              - pyamtrack.materials.Material object.
            Defaults to 1 (Liquid water).
        eps : float or array_like, optional
            Fraction of primary fluence contributing to the tail of the energy spectrum.
            If negative, libamtrack uses the default value 0.03. Default: -1.0.
        cartesian_product: bool
            If True, compute the cartesian product (all possible combinations) across iterable/array
            arguments. If False, compute elementwise (vectorized) results.

        Returns
        -------
        float or numpy.ndarray
            Dose [Gy] at depth. Returns:
              - float if all inputs are scalars,
              - numpy.ndarray if any input is a NumPy array or when cartesian_product=True,
              - list-like behavior depends on wrapper implementation (but typically numpy.ndarray).

        Raises
        ------
        TypeError
            If material is neither an int nor a pyamtrack.materials.Material object,
            or if unsupported input types are provided.
        ValueError
            For invalid model/material IDs or invalid inputs, depending on underlying libamtrack checks.
      )pbdoc");

  m.def("let_dose", &let_dose, R"pbdoc(
        Calculate the LET dose using Wilkens' model.

        This function computes the LET dose at a given depth for a proton beam
        using the analytical model developed by Wilkens. It accounts for energy
        spread and material properties.

        Args:
            depth_cm (float or list[float]): Depth(s) in centimeters where the LET dose is calculated.
            material (int): Material ID (e.g., 1 for liquid water).
            energy_MeV (float): Initial proton energy in MeV.
            energy_spread_MeV (float): Energy spread of the proton beam in MeV.

        Returns:
            float or list[float]: The calculated LET dose at the specified depth(s).

        Raises:
            TypeError: If `depth_cm` is not a float or list of floats.
      )pbdoc");
}
