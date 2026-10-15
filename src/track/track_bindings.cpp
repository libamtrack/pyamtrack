#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "track_radial.h"

namespace nb = nanobind;

NB_MODULE(track, m) {
    m.doc() = "Functions for calculating track structure and radial dose distributions.";

    nb::enum_<RadialDoseDistributionModel>(m, "RadialDoseDistributionModel")
        .value("TEST", RadialDoseDistributionModel::Test)
        .value("KATZ_POINT", RadialDoseDistributionModel::KatzPoint)
        .value("GEISS", RadialDoseDistributionModel::Geiss)
        .value("KATZ_SITE", RadialDoseDistributionModel::KatzSite)
        .value("CUCINOTTA_POINT", RadialDoseDistributionModel::CucinottaPoint)
        .value("KATZ_EXTENDED_TARGET", RadialDoseDistributionModel::KatzExtendedTarget)
        .value("CUCINOTTA_EXTENDED_TARGET", RadialDoseDistributionModel::CucinottaExtendedTarget)
        .value("RADICAL_DIFFUSION", RadialDoseDistributionModel::RadicalDiffusion)
        .export_values();

    m.def("track_width", &track_width, nb::arg("energy_MeV"), nb::arg("material"), nb::arg("model"),
          nb::arg("cartesian_product") = false,
          R"pbdoc(
          Calculate the maximum track width in meters.

          Parameters
          ----------
          energy_MeV : float or array_like
              The kinetic energy of the particle in MeV.
          material : int, Material, list[int | Material], or numpy int array
              The material ID or Material object.
          model : RadialDoseDistributionModel
              The radial dose distribution model to use.
          cartesian_product : bool, optional
              If True, compute all combinations of iterable/array arguments.
              If False, compute elementwise. Default: False.

          Returns
          -------
          float or numpy.ndarray
              The maximum track width in meters.
      )pbdoc");
}