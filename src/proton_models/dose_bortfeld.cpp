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

using ids_getter = std::function<int(const nb::object&)>;

static nb::object get_id(const nb::object& object, const ids_getter& getter) {
  if (nb::isinstance<nb::list>(object)) {
    auto list = nb::cast<nb::list>(object);
    nb::list id;
    for (int i = 0; i < nb::len(list); i++) {
      id.append(getter(nb::cast(list[i])));
    }
    return nb::cast(id);
  } else if (check_int_dtype(object)) {
    return object;  // int numpy array stays as-is
  } else if (nb::isinstance<nb::ndarray<>>(object)) {
    throw nb::type_error("numpy arrays of type other than int unsupported");
  } else {
    int id = getter(object);
    return nb::cast(id);
  }
}

nb::object dose_bortfeld(const nb::object& z_cm, const nb::object& fluence_cm2, const nb::object& E_MeV,
                         const nb::object& sigma_E_MeV, const nb::object& material, const nb::object& eps,
                         bool cartesian_product) {
  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(z_cm);
  arguments_vector.push_back(fluence_cm2);
  arguments_vector.push_back(E_MeV);
  arguments_vector.push_back(sigma_E_MeV);

  // normalize material into int IDs (scalar/list/int ndarray supported)
  arguments_vector.push_back(get_id(material, process_material));

  arguments_vector.push_back(eps);

  auto dose_bortfeld_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 6) {
      throw std::invalid_argument("Input vector must have at least six elements.");
    }

    double z = variant_cast<double>(vec[0]);
    double fluence = variant_cast<double>(vec[1]);
    double E = variant_cast<double>(vec[2]);
    double sigma_E = variant_cast<double>(vec[3]);
    int mat_id = variant_cast<int>(vec[4]);
    double eps_val = variant_cast<double>(vec[5]);

    return AT_dose_Bortfeld_Gy_single(z, fluence, E, sigma_E, (long)mat_id, eps_val);
  };

  if (cartesian_product) return wrap_cartesian_product_function(dose_bortfeld_vector, arguments_vector);
  return wrap_multiargument_function(dose_bortfeld_vector, arguments_vector);
}
