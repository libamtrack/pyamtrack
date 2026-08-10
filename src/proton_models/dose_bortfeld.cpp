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

nb::object dose_bortfeld(const nb::object& z_cm, const nb::object& entrance_dose_Gy, const nb::object& E_MeV,
                         const nb::object& sigma_E_fraction, const nb::object& material, const nb::object& eps,
                         bool cartesian_product) {
  const double entrance_dose   = nb::cast<double>(entrance_dose_Gy);
  const double E          = nb::cast<double>(E_MeV);
  const double sigma_frac = nb::cast<double>(sigma_E_fraction);
  const long   mat_id     = static_cast<long>(get_material_id(material));
  const double eps_val    = nb::cast<double>(eps);

  if (E < 0.1 || E > 10000.0) throw std::invalid_argument("E_MeV must be in [0.1, 10000.0]");
  if (sigma_frac <= 0.0 || sigma_frac >= 1.0) throw std::invalid_argument("sigma_E_fraction must be in (0,1)");
  if (eps_val < 0.0 || eps_val >= 1.0) throw std::invalid_argument("eps must be in [0,1)");
  if (mat_id < 1 || mat_id > 24) throw std::invalid_argument("material ID must be in [1,24]");
  if (entrance_dose < 0.0) throw std::invalid_argument("entrance_dose_Gy must be >= 0");

  const double sigma_E_MeV = sigma_frac * E;

  const bool z_is_scalar = nb::isinstance<nb::float_>(z_cm) || nb::isinstance<nb::int_>(z_cm);

  if (z_is_scalar) {
    const double z = nb::cast<double>(z_cm);
    if (z < 0.0) throw std::invalid_argument("z_cm must be >= 0");
    const double D1 = AT_dose_Bortfeld_Gy_single(0.0, 1.0, E, sigma_E_MeV, mat_id, eps_val);
    if (D1 <= 0.0) throw std::runtime_error("Cannot normalize fluence: non-positive entrance dose for unit fluence.");
    const double fluence = entrance_dose / D1;
    const double d = AT_dose_Bortfeld_Gy_single(z, fluence, E, sigma_E_MeV, mat_id, eps_val);
    return nb::cast(d);
  }

  // array/list -> multi
  std::vector<double> z_vec = nb::cast<std::vector<double>>(z_cm);
  if (z_vec.empty()) throw std::invalid_argument("z_cm array cannot be empty.");
  for (double z : z_vec) if (z < 0.0) throw std::invalid_argument("z_cm must be >= 0");

  if (z_vec.size() > static_cast<size_t>(std::numeric_limits<long>::max()))
    throw std::overflow_error("z_cm length exceeds C long range.");
  const long n = static_cast<long>(z_vec.size());

  std::vector<double> dose(z_vec.size(), 0.0);
  const double D1 = AT_dose_Bortfeld_Gy_single(0.0, 1.0, E, sigma_E_MeV, mat_id, eps_val);
  if (D1 <= 0.0) throw std::runtime_error("Cannot normalize fluence: non-positive entrance dose for unit fluence.");
  const double fluence = entrance_dose / D1;
  AT_dose_Bortfeld_Gy_multi(n, z_vec.data(), fluence, E, sigma_E_MeV, mat_id, eps_val, dose.data());

  double* data = new double[dose.size()];
  std::copy(dose.begin(), dose.end(), data);
  nb::capsule owner(data, [](void* p) noexcept { delete[] static_cast<double*>(p); });
  auto arr = nb::ndarray<double, nb::numpy>(data, {dose.size()}, owner);
  return arr.cast();
}
