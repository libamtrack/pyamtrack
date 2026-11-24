#include "electron_range.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ElectronRange.h"  // Contains AT_max_electron_range_m definition
}
std::vector<std::string> get_models() {
  std::vector<std::string> names;
  for (const auto& [name, model] : ELECTRON_RANGE_MODELS) {
    names.push_back(name);
  }
  return names;
}

ElectronRangeModel parse_model_name(const std::string& model_name) {
  auto it = ELECTRON_RANGE_MODELS.find(model_name);
  if (it == ELECTRON_RANGE_MODELS.end()) {
    throw std::invalid_argument("Unknown model name: " + model_name);
  }
  return it->second;
}

int get_model_id(const std::string& model_name) {
  return static_cast<int>(parse_model_name(model_name));
}

namespace {

ElectronRangeModel parse_model_scalar(const nb::object& model) {
  if (PyBool_Check(model.ptr())) {
    throw nb::type_error("model must be an ElectronRangeModel, string, or integer (not bool)");
  }

  if (nb::isinstance<ElectronRangeModel>(model)) {
    return nb::cast<ElectronRangeModel>(model);
  }

  if (nb::isinstance<nb::str>(model)) {
    return parse_model_name(nb::cast<std::string>(model));
  }

  if (nb::isinstance<nb::int_>(model)) {
    const int model_id = nb::cast<int>(model);
    for (const auto& [name, registered_model] : ELECTRON_RANGE_MODELS) {
      if (static_cast<int>(registered_model) == model_id) {
        return registered_model;
      }
    }
    throw std::invalid_argument("Invalid electron range model ID: " + std::to_string(model_id));
  }

  throw nb::type_error("model must be an ElectronRangeModel, string, or integer");
}

}  // namespace

void validate_model_argument(const nb::object& argument) {
  if (nb::isinstance<nb::list>(argument)) {
    const nb::list values = nb::cast<nb::list>(argument);
    for (size_t i = 0; i < values.size(); ++i) {
      validate_model_argument(values[i]);
    }
    return;
  }

  if (nb::isinstance<nb::ndarray<>>(argument)) {
    if (!check_int_dtype(argument)) {
      throw nb::type_error("model NumPy arrays must have an integer dtype");
    }
    validate_model_argument(argument.attr("tolist")());
    return;
  }

  parse_model_scalar(argument);
}

nb::object parse_model_argument(const nb::object& argument) {
  if (nb::isinstance<nb::list>(argument)) {
    const nb::list values = nb::cast<nb::list>(argument);
    nb::list parsed_values;
    for (size_t i = 0; i < values.size(); ++i) {
      parsed_values.append(parse_model_argument(values[i]));
    }
    return parsed_values;
  }

  if (nb::isinstance<nb::ndarray<>>(argument)) {
    if (!check_int_dtype(argument)) {
      throw nb::type_error("model NumPy arrays must have an integer dtype");
    }
    validate_model_argument(argument.attr("tolist")());
    return argument;
  }

  return nb::cast(static_cast<int>(parse_model_scalar(argument)));
}

nb::object electron_range(const nb::object& energy_MeV, const nb::object& material, const nb::object& model,
                          const bool cartesian_product) {
  validate_material_argument(material);
  validate_model_argument(model);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(parse_model_argument(model));

  auto electron_range_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 3) {
      throw std::invalid_argument("Input vector must have at least three elements.");
    }
    double energy = variant_cast<double>(vec[0]);
    int mat_id = variant_cast<int>(vec[1]);
    int model_id = variant_cast<int>(vec[2]);

    if (!std::isfinite(energy) || energy < 0.0) {
      throw std::invalid_argument("energy_MeV must be non-negative and finite");
    }

    return AT_max_electron_range_m(energy, mat_id, model_id);
  };

  if (cartesian_product) return wrap_cartesian_product_function(electron_range_vector, arguments_vector);
  return wrap_multiargument_function(electron_range_vector, arguments_vector);
}
