#include "electron_range.h"

#include <stdexcept>  // For std::runtime_error
#include <string>     // For std::string
#include <vector>     // For std::vector

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ElectronRange.h"  // Contains AT_max_electron_range_m definition
}
using ids_getter = std::function<int(const nb::object&)>;

std::vector<ElectronRangeModel> get_electron_range_models() {
  std::vector<ElectronRangeModel> models;
  for (const auto& [name, model] : ELECTRON_RANGE_MODELS) {
    models.push_back(model);
  }
  return models;
}

ElectronRangeModel get_model_enum(const std::string& model_name) {
  auto it = ELECTRON_RANGE_MODELS.find(model_name);
  if (it == ELECTRON_RANGE_MODELS.end()) {
    throw std::runtime_error("Unknown model name: " + model_name);
  }
  return it->second;
}

ElectronRangeModel parse_electron_range_model(const nb::object& model) {
  ElectronRangeModel model_enum;
  if (nb::cast<ElectronRangeModel>(model, &model_enum)) {
    return model_enum;
  } else {
    throw std::runtime_error("Invalid model type. Must be an ElectronRangeModel enum.");
  }
}

static nb::object get_id(const nb::object& object, const ids_getter& getter) {
  if (nb::isinstance<nb::list>(object)) {
    auto list = nb::cast<nb::list>(object);
    nb::list id;
    for (int i = 0; i < nb::len(list); i++) {
      id.append(getter(nb::cast(list[i])));
    }
    return nb::cast(id);
  } else if (check_int_dtype(object)) {
    return object;
  } else if (nb::isinstance<nb::ndarray<>>(object)) {
    throw nb::type_error("numpy arrays of type other than int unsupported");
  } else {
    int id = getter(object);
    return nb::cast(id);
  }
}

nb::object electron_range(const nb::object& energy_MeV, const nb::object& material, const nb::object& model,
                          const bool cartesian_product) {
  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(get_id(material, process_material));  // unifying materials to int
  arguments_vector.push_back(get_id(model, process_model));        // unifying models to int
  auto electron_range_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 3) {
      throw std::invalid_argument("Input vector must have at least three elements.");
    }
    double energy = variant_cast<double>(vec[0]);
    int mat_id = variant_cast<int>(vec[1]);
    int model_id = variant_cast<int>(vec[2]);

    return AT_max_electron_range_m(energy, mat_id, model_id);
  };
  if (cartesian_product) return wrap_cartesian_product_function(electron_range_vector, arguments_vector);
  return wrap_multiargument_function(electron_range_vector, arguments_vector);
}
