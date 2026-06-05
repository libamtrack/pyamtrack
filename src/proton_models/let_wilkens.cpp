#include "let_wilkens.h"

#include <iostream>

extern "C" {
#include "AT_ProtonAnalyticalModels.h"
}

nb::object let_dose(nb::object depth_cm, long material, double energy_MeV, double energy_spread_MeV) {
  if (nb::isinstance<nb::list>(depth_cm)) {
    auto list = nb::cast<nb::list>(depth_cm);

    std::vector<double> depths;
    depths.reserve(nb::len(list));
    for (auto item : list) {
      depths.push_back(nb::cast<double>(item));
    }

    std::vector<double> LET_keV_um(depths.size());
    AT_LET_d_Wilkens_keV_um_multi(
      static_cast<long>(depths.size()),
      depths.data(),
      energy_MeV,
      energy_spread_MeV,
      material,
      LET_keV_um.data()
    );

    return nb::cast(LET_keV_um);

  } else if (nb::isinstance<nb::ndarray<>>(depth_cm)){
    auto arr = nb::cast<nb::ndarray<double, nb::numpy>>(depth_cm);
    
    std::vector<double> depths(arr.data(), arr.data() + arr.size());

    std::vector<double> LET_keV_um(depths.size());
    AT_LET_d_Wilkens_keV_um_multi(
      static_cast<long>(depths.size()),
      depths.data(),
      energy_MeV,
      energy_spread_MeV,
      material,
      LET_keV_um.data()
    );

    return nb::cast(LET_keV_um);

  } else if (nb::isinstance<nb::float_>(depth_cm) || nb::isinstance<nb::int_>(depth_cm)) {
    double depth = nb::cast<double>(depth_cm);
    double result = AT_LET_d_Wilkens_keV_um_single(depth, energy_MeV, energy_spread_MeV, material);
    return nb::cast(result);
  } else {
    throw nb::type_error("Input must be a float or a list of floats");
  }
}