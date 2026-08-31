#include "let_wilkens.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ProtonAnalyticalModels.h"
}

nb::object let_wilkens(const nb::object& depth_cm, const nb::object& energy_MeV,
                       const nb::object& energy_spread_fraction, const nb::object& material,
                       const nb::object& averaging, bool cartesian_product) {
  bool dose_averaged;
  if (nb::isinstance<Averaging>(averaging)) {
    dose_averaged = (nb::cast<Averaging>(averaging) == Averaging::Dose);
  } else if (nb::isinstance<nb::str>(averaging)) {
    std::string avg_str = nb::cast<std::string>(averaging);
    std::transform(avg_str.begin(), avg_str.end(), avg_str.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (avg_str == "dose")
      dose_averaged = true;
    else if (avg_str == "track")
      dose_averaged = false;
    else
      throw std::invalid_argument("averaging must be \"dose\" or \"track\", got: " + avg_str);
  } else {
    throw nb::type_error("averaging must be an Averaging enum value or a string");
  }

  validate_material_argument(material);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(depth_cm);
  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(energy_spread_fraction);
  arguments_vector.push_back(parse_material_argument(material));

  auto let_wilkens_vector = [dose_averaged](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 4) {
      throw std::invalid_argument("Input vector must have at least four elements.");
    }

    double depth = variant_cast<double>(vec[0]);
    double E = variant_cast<double>(vec[1]);
    double spread_frac = variant_cast<double>(vec[2]);
    int mat_id = variant_cast<int>(vec[3]);

    if (depth < 0.0)
      throw std::invalid_argument("depth_cm must be >= 0. Negative depth has no physical meaning: got " +
                                  std::to_string(depth));
    if (E < 0.1 || E > 10000.0)
      throw std::invalid_argument("energy_MeV must be in range [0.1, 10000.0], got: " + std::to_string(E));
    if (spread_frac <= 0.0 || spread_frac >= 1.0)
      throw std::invalid_argument("energy_spread_fraction must be in range (0, 1), got: " +
                                  std::to_string(spread_frac));

    const double energy_spread_MeV = spread_frac * E;

    return dose_averaged ? AT_LET_d_Wilkens_keV_um_single(depth, E, energy_spread_MeV, static_cast<long>(mat_id))
                         : AT_LET_t_Wilkens_keV_um_single(depth, E, energy_spread_MeV, static_cast<long>(mat_id));
  };

  if (cartesian_product) return wrap_cartesian_product_function(let_wilkens_vector, arguments_vector);
  return wrap_multiargument_function(let_wilkens_vector, arguments_vector);
}
