#include <nanobind/nanobind.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "dose_bortfeld.h"
#include "let_wilkens.h"

namespace nb = nanobind;

NB_MODULE(proton_models, m) {
  m.doc() = "Analytical proton beam models (e.g., Bragg curve approximations, LET, RBE).";

  m.def("dose_bortfeld", &dose_bortfeld, nb::arg("depth_cm"), nb::arg("fluence_cm2"), nb::arg("energy_MeV"),
        nb::arg("energy_spread_fraction") = 0.01, nb::arg("material") = 1, nb::arg("eps") = 0.03,
        nb::arg("cartesian_product") = false,
        R"pbdoc(
        Compute dose at depth for proton beams using the analytical model of Bortfeld (1997).

        Reference: T. Bortfeld, "An analytical approximation of the Bragg curve for therapeutic
        proton beams", Med. Phys. 24(12), 2024 (1997).

        Parameters
        ----------
        depth_cm : float or array_like
            Depth in medium [cm]. Must be >= 0. Can be a float, a Python list, or a NumPy array.
        fluence_cm2 : float or array_like
            Proton fluence [1/cm^2]. Can be a float, a Python list, or a NumPy array.
        energy_MeV : float or array_like
            Initial kinetic energy [MeV]. Must be in [0.1, 10000.0].
            Can be a float, a Python list, or a NumPy array.
        energy_spread_fraction : float or array_like
            Energy spread (standard deviation) expressed as a fraction of energy_MeV. Must be in (0, 1).
            For example, 0.01 means a 1% energy spread. Internally converted to
            sigma_E_MeV = energy_spread_fraction * E_MeV before calling libamtrack.
        material : int, Material, list[int | Material], or numpy int array, optional
            Any material ID listed in pyamtrack.materials.valid_material_ids, or a
            pyamtrack.materials.Material object. Default: 1 (liquid water).
        eps : float or array_like, optional
            Fraction of primary fluence contributing to the nuclear interaction tail. Must be
            in [0, 1). Default: 0.03.
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
            If material is not an int, Material, list, or int numpy array, or if other inputs
            are unsupported types.
        ValueError
            If z_cm < 0, E_MeV outside [0.1, 10000.0], sigma_E_fraction outside (0, 1),
            eps outside [0, 1), or a material ID is not a known material.
      )pbdoc");

  m.def("let_wilkens", &let_wilkens, nb::arg("depth_cm"), nb::arg("energy_MeV"),
        nb::arg("energy_spread_fraction") = 0.01, nb::arg("material") = 1, nb::arg("averaging") = "dose",
        nb::arg("cartesian_product") = false,
        R"pbdoc(
        Compute LET at depth for proton beams using the analytical model of Wilkens & Oelfke (2003).

        Reference: J. J. Wilkens & U. Oelfke, "Analytical linear energy transfer calculations
        for proton therapy", Med. Phys. 30(5), 806 (2003).

        Parameters
        ----------
        depth_cm : float or array_like
            Depth(s) in medium [cm]. Must be >= 0; negative values have no physical
            meaning (the beam does not exist before the material surface).
            Can be a float, a Python list, or a NumPy array.
        energy_MeV : float or array_like
            Initial kinetic energy of the proton beam [MeV]. Must be in [0.1, 10000.0].
            Can be a float, a Python list, or a NumPy array.
        energy_spread_fraction : float or array_like, optional
            Energy spread (standard deviation) expressed as a fraction of energy_MeV.
            Must be in (0, 1). For example, 0.01 means a 1% energy spread.
            Internally converted to sigma_E = energy_spread_fraction * energy_MeV before
            calling libamtrack. Can be a float, a Python list, or a NumPy array.
            Default: 0.01.
        material : int, Material, list[int | Material], or numpy int array, optional
            Any material ID listed in pyamtrack.materials.valid_material_ids, or a
            pyamtrack.materials.Material object. Default: 1 (liquid water).
        averaging : str or Averaging, optional
            LET averaging convention:
              - "dose"  : dose-averaged LET (LET_d) — weighted by dose contribution.
                          Standard choice for RBE calculations. Calls AT_LET_d_Wilkens_keV_um.
              - "track" : track-averaged LET (LET_t) — plain fluence-weighted mean of stopping power.
                          Calls AT_LET_t_Wilkens_keV_um.
            The Averaging.DOSE / Averaging.TRACK enum members are accepted as well.
            Default: "dose".
        cartesian_product : bool, optional
            If True, compute all combinations of iterable/array arguments (cartesian product).
            If False, compute elementwise. Default: False.

        Returns
        -------
        float or numpy.ndarray
            LET [keV/µm] at the specified depth(s). LET_d is generally >= LET_t up to
            numerical precision; the difference grows near the Bragg peak.

        Raises
        ------
        ValueError
            If averaging is not "dose" or "track", or any depth_cm value is < 0,
            or energy_MeV is outside [0.1, 10000.0], or energy_spread_fraction is
            outside (0, 1), or a material ID is not a known material.
        TypeError
            If depth_cm is not a float, list, or NumPy array, or material is not
            an int, Material, list, or NumPy array.
      )pbdoc");
	// enum constucted by chaining
  nb::enum_<Averaging>(m, "Averaging").value("DOSE", Averaging::Dose).value("TRACK", Averaging::Track).export_values();
}
