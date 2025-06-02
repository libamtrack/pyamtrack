#include "electron_range.h"

#include <stdexcept>  // For std::runtime_error
#include <string>     // For std::string
#include <vector>     // For std::vector

#include "../wrapper_template.h"  // Should contain wrap_function

extern "C" {
#include "AT_ElectronRange.h"  // Contains AT_max_electron_range_m definition
}
using ids_getter = std::function<int(const nb::object&)>;

std::vector<std::string> get_models() {
  std::vector<std::string> names;
  for (const auto& [name, id] : STOPPING_MODELS) {
    names.push_back(name);
  }
  return names;
}

int get_model_id(const std::string& model_name) {
  auto it = STOPPING_MODELS.find(model_name);
  if (it == STOPPING_MODELS.end()) {
    throw std::runtime_error("Unknown model name: " + model_name);
  }
  return it->second;
}

int process_model(const nb::object& model) {
  int model_id = 0;
  if (nb::isinstance<nb::str>(model)) {
    std::string model_name = nb::cast<std::string>(model);
    try {
      model_id = get_model_id(model_name);
    } catch (const std::runtime_error& e) {
      throw nb::value_error(e.what());
    }
  } else if (nb::isinstance<nb::int_>(model)) {
    model_id = nb::cast<int>(model);
  } else {
    throw nb::type_error("Model argument must be either an integer or a string");
  }
  return model_id;
}

nb::object get_id(const nb::object& object, const ids_getter& getter) {
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

nb::object electron_range(const nb::object& input, const nb::object& material, const nb::object& model) {
  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(input);
  auto electron_range_vector = [](std::vector<double> vec) -> double {
    if (vec.size() < 3) {
      throw std::invalid_argument("Input vector must have at least three elements.");
    }
    return AT_max_electron_range_m(vec[0], (int)vec[1], (int)vec[2]);
  };
  arguments_vector.push_back(get_id(material, process_material));  // unifying materials to int
  arguments_vector.push_back(get_id(model, process_model));        // unifying models to int
  return wrap_multiargument_function(electron_range_vector, arguments_vector);
}
