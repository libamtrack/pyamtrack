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

nb::object radial_dose(const nb::object& radius_m, const nb::object& energy_MeV_u, const nb::object& particle, 
                    const nb::object& material=nb::cast<int>(1), 
                    const nb::object& rdd_model=nb::cast<RadialDoseDistributionModel>(RadialDoseDistributionModel.KATZ_POINT),
                    const nb::object& rdd_parameters=None, const nb::object& electron_range_model="tabata", const nb::object& source="default", 
                    const nb::object& cartesian_product=False,
)
