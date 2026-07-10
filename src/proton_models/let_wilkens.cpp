#include "let_wilkens.h"

#include <stdexcept>
#include <string>

extern "C" {
#include "AT_ProtonAnalyticalModels.h"
}

nb::object let_wilkens(nb::object depth_cm, long material, double energy_MeV, double energy_spread_MeV,
                       const std::string& averaging) {
  if (averaging != "dose" && averaging != "track") {
    throw std::invalid_argument("averaging must be \"dose\" or \"track\", got: " + averaging);
  }
  const bool dose_averaged = (averaging == "dose");

  auto validate_depths = [](const std::vector<double>& depths) {
    for (double d : depths) {
      if (d < 0.0) {
        throw std::invalid_argument(
          "depth_cm must be >= 0. Negative depth has no physical meaning: got " + std::to_string(d));
      }
    }
  };

  auto call_multi = [&](std::vector<double>& depths) -> nb::object {
    validate_depths(depths);
    std::vector<double> LET_keV_um(depths.size());
    if (dose_averaged) {
      AT_LET_d_Wilkens_keV_um_multi(static_cast<long>(depths.size()), depths.data(),
                                    energy_MeV, energy_spread_MeV, material, LET_keV_um.data());
    } else {
      AT_LET_t_Wilkens_keV_um_multi(static_cast<long>(depths.size()), depths.data(),
                                    energy_MeV, energy_spread_MeV, material, LET_keV_um.data());
    }
    return nb::cast(LET_keV_um);
  };

  if (nb::isinstance<nb::list>(depth_cm)) {
    auto list = nb::cast<nb::list>(depth_cm);
    std::vector<double> depths;
    depths.reserve(nb::len(list));
    for (auto item : list) {
      depths.push_back(PyFloat_AsDouble(item.ptr()));
      if (PyErr_Occurred()) nb::raise_python_error();
    }
    return call_multi(depths);

  } else if (nb::isinstance<nb::ndarray<>>(depth_cm)) {
    auto arr = nb::cast<nb::ndarray<double, nb::numpy>>(depth_cm);
    std::vector<double> depths(arr.data(), arr.data() + arr.size());
    return call_multi(depths);

  } else if (nb::isinstance<nb::float_>(depth_cm) || nb::isinstance<nb::int_>(depth_cm)) {
    double depth = nb::cast<double>(depth_cm);
    if (depth < 0.0) {
      throw std::invalid_argument(
        "depth_cm must be >= 0. Negative depth has no physical meaning: got " + std::to_string(depth));
    }
    double result = dose_averaged
      ? AT_LET_d_Wilkens_keV_um_single(depth, energy_MeV, energy_spread_MeV, material)
      : AT_LET_t_Wilkens_keV_um_single(depth, energy_MeV, energy_spread_MeV, material);
    return nb::cast(result);

  } else {
    throw nb::type_error("depth_cm must be a float, list, or NumPy array");
  }
}