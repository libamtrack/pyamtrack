#include "dose_bortfeld.h"

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ProtonAnalyticalModels.h"
}

nb::object dose_bortfeld(const nb::object& depth_cm, const nb::object& fluence_cm2, const nb::object& energy_MeV,
                         const nb::object& energy_spread_fraction, const nb::object& material, const nb::object& eps,
                         bool cartesian_product) {
  validate_material_argument(material);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(depth_cm);
  arguments_vector.push_back(fluence_cm2);
  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(energy_spread_fraction);
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(eps);

  auto dose_bortfeld_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 6) {
      throw std::invalid_argument("Input vector must have at least six elements.");
    }

    double depth_cm_ = variant_cast<double>(vec[0]);
    double fluence_cm2_ = variant_cast<double>(vec[1]);
    double energy_MeV_ = variant_cast<double>(vec[2]);
    double energy_spread_fraction_ = variant_cast<double>(vec[3]);
    int material_id = variant_cast<int>(vec[4]);
    double eps_ = variant_cast<double>(vec[5]);

    if (depth_cm_ < 0.0) throw std::invalid_argument("depth_cm must be >= 0, got: " + std::to_string(depth_cm_));
    if (energy_MeV_ < 0.1 || energy_MeV_ > 10000.0)
      throw std::invalid_argument("energy_MeV must be in [0.1, 10000.0], got: " + std::to_string(energy_MeV_));
    if (energy_spread_fraction_ <= 0.0 || energy_spread_fraction_ >= 1.0)
      throw std::invalid_argument("energy_spread_fraction must be in (0, 1), got: " + std::to_string(energy_spread_fraction_));
    if (eps_ < 0.0 || eps_ >= 1.0)
      throw std::invalid_argument("eps must be in [0, 1), got: " + std::to_string(eps_));

    const double sigma_E_MeV = energy_spread_fraction_ * energy_MeV_;
    return AT_dose_Bortfeld_Gy_single(depth_cm_, fluence_cm2_, energy_MeV_, sigma_E_MeV, (long)material_id, eps_);
  };

  if (cartesian_product) return wrap_cartesian_product_function(dose_bortfeld_vector, arguments_vector);
  return wrap_multiargument_function(dose_bortfeld_vector, arguments_vector);
}
