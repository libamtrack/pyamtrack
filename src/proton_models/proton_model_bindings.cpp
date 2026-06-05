#include <nanobind/nanobind.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "let_wilkens.h"

namespace nb = nanobind;

NB_MODULE(proton_models, m) {
  m.doc() = "proton models for calculating LET and dose";

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