#include "dose_bortfeld.h"

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ProtonAnalyticalModels.h"  // adjust to the correct header providing AT_dose_Bortfeld_Gy_*
}

nb::object dose_bortfeld(const nb::object& z_cm, const nb::object& fluence_cm2, const nb::object& E_MeV,
                         const nb::object& sigma_E_fraction, const nb::object& material, const nb::object& eps,
                         bool cartesian_product) {
  validate_material_argument(material);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(z_cm);
  arguments_vector.push_back(fluence_cm2);
  arguments_vector.push_back(E_MeV);
  arguments_vector.push_back(sigma_E_fraction);
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(eps);

  auto dose_bortfeld_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 6) {
      throw std::invalid_argument("Input vector must have at least six elements.");
    }

    double z          = variant_cast<double>(vec[0]);
    double fluence    = variant_cast<double>(vec[1]);
    double E          = variant_cast<double>(vec[2]);
    double sigma_frac = variant_cast<double>(vec[3]);
    int    mat_id     = variant_cast<int>(vec[4]);
    double eps_val    = variant_cast<double>(vec[5]);

    if (z < 0.0)
      throw std::invalid_argument("z_cm must be >= 0, got: " + std::to_string(z));
    if (E < 0.1 || E > 10000.0)
      throw std::invalid_argument("E_MeV must be in [0.1, 10000.0], got: " + std::to_string(E));
    if (sigma_frac <= 0.0 || sigma_frac >= 1.0)
      throw std::invalid_argument("sigma_E_fraction must be in (0, 1), got: " + std::to_string(sigma_frac));
    if (eps_val < 0.0 || eps_val >= 1.0)
      throw std::invalid_argument("eps must be in [0, 1), got: " + std::to_string(eps_val));

    const double sigma_E_MeV = sigma_frac * E;

    return AT_dose_Bortfeld_Gy_single(z, fluence, E, sigma_E_MeV, (long)mat_id, eps_val);
  };

  if (cartesian_product) return wrap_cartesian_product_function(dose_bortfeld_vector, arguments_vector);
  return wrap_multiargument_function(dose_bortfeld_vector, arguments_vector);
}
