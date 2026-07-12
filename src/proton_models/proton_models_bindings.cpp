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
        nb::arg("sigma_E_MeV"), nb::arg("material") = 1, nb::arg("eps") = 0.03, nb::arg("cartesian_product") = false,
        R"pbdoc(
        Compute dose at depth for proton beams using the analytical model of Bortfeld (1997).

        Reference: T. Bortfeld, "An analytical approximation of the Bragg curve for therapeutic
        proton beams", Med. Phys. 24(12), 2024 (1997).

        Parameters
        ----------
        z_cm : float or array_like
            Depth in medium [cm]. Can be a float, a Python list, or a NumPy array.
        fluence_cm2 : float or array_like
            Proton fluence [1/cm^2]. Can be a float, a Python list, or a NumPy array.
        E_MeV : float or array_like
            Initial kinetic energy [MeV]. Can be a float, a Python list, or a NumPy array.
        sigma_E_MeV : float or array_like
            Kinetic energy spread (standard deviation) [MeV]. If negative, defaults to
            0.01 * E_MeV.
        material : int or Material, optional
            Material ID or pyamtrack.materials.Material object. Default: 1 (liquid water).
        eps : float or array_like, optional
            Fraction of primary fluence contributing to the tail of the energy spectrum.
            Default: 0.03.
        cartesian_product : bool, optional
            If True, compute all combinations of iterable/array arguments (cartesian product).
            If False, compute elementwise. Default: False.

        Returns
        -------
        float or numpy.ndarray
            Dose [Gy] at the specified depth(s).

        Raises
        ------
        TypeError
            If material is not an int or pyamtrack.materials.Material, or inputs are unsupported types.
        ValueError
            For invalid material IDs or out-of-range inputs.
      )pbdoc");

  m.def("let_wilkens", &let_wilkens,
        nb::arg("depth_cm"), nb::arg("material"), nb::arg("energy_MeV"), nb::arg("energy_spread_fraction"),
        nb::arg("averaging") = "dose",
        R"pbdoc(
        Compute LET at depth for proton beams using the analytical model of Wilkens & Oelfke (2003).

        Reference: J. J. Wilkens & U. Oelfke, "Analytical linear energy transfer calculations
        for proton therapy", Med. Phys. 30(5), 806 (2003).

        Parameters
        ----------
        depth_cm : float, list[float], or numpy.ndarray
            Depth(s) in medium [cm]. Must be >= 0; negative values have no physical
            meaning (the beam does not exist before the material surface).
        material : int
            Material ID (e.g., 1 for liquid water).
        energy_MeV : float
            Initial kinetic energy of the proton beam [MeV].
        energy_spread_MeV : float
            Energy spread (standard deviation) of the proton beam [MeV].
            If negative, defaults to 0.01 * energy_MeV.
        averaging : str, optional
            LET averaging convention:
              - "dose"  : dose-averaged LET (LET_d) — weighted by dose contribution.
                          Standard choice for RBE calculations. Calls AT_LET_d_Wilkens_keV_um.
              - "track" : track-averaged LET (LET_t) — plain fluence-weighted mean of stopping power.
                          Calls AT_LET_t_Wilkens_keV_um.
            Default: "dose".

        Returns
        -------
        float or list[float]
            LET [keV/µm] at the specified depth(s). LET_d >= LET_t always; the difference
            grows near the Bragg peak.

        Raises
        ------
        ValueError
            If averaging is not "dose" or "track", or if any depth_cm value is < 0.
        TypeError
            If depth_cm is not a float, list, or NumPy array.
      )pbdoc");
}
