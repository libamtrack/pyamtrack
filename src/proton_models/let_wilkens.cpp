#include "let_wilkens.h"

#include <stdexcept>
#include <string>

extern "C" {
#include "AT_ProtonAnalyticalModels.h"
}

nb::object let_wilkens(nb::object depth_cm, nb::object material, double energy_MeV, double energy_spread_fraction,
                      nb::object averaging) {

  auto validate_depths = [](const std::vector<double>& depths) {
    for (double d : depths) {
      if (d < 0.0) {
        throw std::invalid_argument(
          "depth_cm must be >= 0. Negative depth has no physical meaning: got " + std::to_string(d));
      }
    }
  };
  
  if (energy_MeV < 0.1 || energy_MeV > 10000.0) {
    throw std::invalid_argument("energy_MeV must in range [0.1, 10000.0], got: " + std::to_string(energy_MeV));
  }
  if (!(0.0 < energy_spread_fraction && energy_spread_fraction < 1.0)) {
    throw std::invalid_argument("energy_spread_fraction must be in range (0, 1), got: " + std::to_string(energy_spread_fraction));
  }

  bool dose_averaged;
  if (nb::isinstance<Averaging>(averaging)) {
      dose_averaged = (nb::cast<Averaging>(averaging) == Averaging::Dose);
  } else if (nb::isinstance<nb::str>(averaging)) {
      std::string avg_str = nb::cast<std::string>(averaging);
      if (avg_str == "dose")       dose_averaged = true;
      else if (avg_str == "track") dose_averaged = false;
      else throw std::invalid_argument(
          "averaging must be \"dose\" or \"track\", got: " + avg_str);
  } else {
      throw nb::type_error("averaging must be an Averaging enum value or a string");
  }
  const double energy_spread_MeV = energy_spread_fraction * energy_MeV;

  long material_id = get_material_id(material);

  auto call_multi = [&](std::vector<double>& depths) -> nb::object {
    validate_depths(depths);
    std::vector<double> LET_keV_um(depths.size());
    if (dose_averaged) {
      AT_LET_d_Wilkens_keV_um_multi(static_cast<long>(depths.size()), depths.data(),
                                    energy_MeV, energy_spread_MeV, material_id, LET_keV_um.data());
    } else {
      AT_LET_t_Wilkens_keV_um_multi(static_cast<long>(depths.size()), depths.data(),
                                    energy_MeV, energy_spread_MeV, material_id, LET_keV_um.data());
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

    std::vector<nb::object> arguments_vector;

    double result = dose_averaged
      ? AT_LET_d_Wilkens_keV_um_single(depth, energy_MeV, energy_spread_MeV, material_id)
      : AT_LET_t_Wilkens_keV_um_single(depth, energy_MeV, energy_spread_MeV, material_id);
    return nb::cast(result);

  } else {
    throw nb::type_error("depth_cm must be a float, list, or NumPy array");
  }
}