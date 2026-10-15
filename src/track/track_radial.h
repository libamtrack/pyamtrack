#include <nanobind/nanobind.h>

namespace nb = nanobind;

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

nb::object track_width(const nb::object& energy_MeV, const nb::object& material, const nb::object& model, bool cartesian_product);

