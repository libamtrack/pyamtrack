#include "track_radial.h"

#include <gsl/gsl_integration.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "../materials/materials.h"
#include "../particles/particle_arguments.h"
#include "../stopping/electron_range.h"
#include "../stopping/stopping_power_source.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ElectronRange.h"
#include "AT_RDD.h"
}

namespace {

enum class RadialDoseDistributionModel {
  Test = 1,
  KatzPoint = 2,
  Geiss = 3,
  KatzSite = 4,
  CucinottaPoint = 5,
  KatzExtendedTarget = 6,
  CucinottaExtendedTarget = 7,
  RadicalDiffusion = 8
};

// libamtrack energies above this limit trigger assertions in the electron-range models.
constexpr double MAXIMUM_ENERGY_MeV_u = 1e6;

enum class TargetCalculation {
  Point = 1,
  Site = 2,
  Extended = 3,
};

bool is_electron_range_model_compatible(const RadialDoseDistributionModel rdd_model, const long electron_range_model) {
  if (electron_range_model < ER_Test || electron_range_model > ER_Kiefer) {
    return false;
  }

  switch (rdd_model) {
    case RadialDoseDistributionModel::KatzPoint:
    case RadialDoseDistributionModel::KatzSite:
    case RadialDoseDistributionModel::KatzExtendedTarget:
      return electron_range_model == ER_ButtsKatz || electron_range_model == ER_Waligorski ||
             electron_range_model == ER_Edmund;

    case RadialDoseDistributionModel::Test:
    case RadialDoseDistributionModel::Geiss:
    case RadialDoseDistributionModel::CucinottaPoint:
    case RadialDoseDistributionModel::CucinottaExtendedTarget:
    case RadialDoseDistributionModel::RadicalDiffusion:
      return true;
  }

  return false;
}

nb::object track_width_katz(
    nb::object radius_m,
    nb::object energy_MeV_u,
    nb::object particle,
    nb::object target="point",
    nb::object material=1,
    rdd_parameters=None,
    nb::object electron_range_model="waligorski",
    source="default",
    bool cartesian_product=False,
    bool allow_bethe_fallback=False,
    bool shell_averaged=False,
) {

}

}