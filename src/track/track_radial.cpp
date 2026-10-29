#include "track_radial.h"

#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>

#include <array>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "../materials/materials.h"
#include "../particles/particle_arguments.h"
#include "../stopping/electron_range.h"
#include "../stopping/stopping_power_source.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_ElectronRange.h"
#include "AT_RDD.h"
}

namespace {

enum class RadialDoseDistributionModel {
  Test = 1,
  KatzPoint = 2,
  Geiss = 3,
  KatzSite = 4,
  CucinottaPoint = 5,
  KatzExtendedTarget = 6,
  CucinottaExtendedTarget = 7,
  RadicalDiffusion = 8
};

// libamtrack energies above this limit trigger assertions in the electron-range models.
constexpr double MAXIMUM_ENERGY_MeV_u = 1e6;

bool is_electron_range_model_compatible(const RadialDoseDistributionModel rdd_model,
                                        const long electron_range_model) {
  if (electron_range_model < ER_Test || electron_range_model > ER_Kiefer) {
    return false;
  }

  switch (rdd_model) {
    case RadialDoseDistributionModel::KatzPoint:
    case RadialDoseDistributionModel::KatzSite:
    case RadialDoseDistributionModel::KatzExtendedTarget:
      return electron_range_model == ER_ButtsKatz || electron_range_model == ER_Waligorski ||
             electron_range_model == ER_Edmund;

    case RadialDoseDistributionModel::Test:
    case RadialDoseDistributionModel::Geiss:
    case RadialDoseDistributionModel::CucinottaPoint:
    case RadialDoseDistributionModel::CucinottaExtendedTarget:
    case RadialDoseDistributionModel::RadicalDiffusion:
      // AT_D_RDD_Gy does not impose additional ER restrictions for
      // these forward-calculation branches.
      return true;
  }

  return false;
}

/**
 * Public description of one RDD model: the Python function name and the public names of the
 * rdd_parameter slots, in the order expected by AT_D_RDD_Gy.
 */
struct RadialDoseModelSpec {
  RadialDoseDistributionModel model;
  const char* function_name;
  std::vector<std::string> parameter_names;
};

const RadialDoseModelSpec KATZ_POINT{RadialDoseDistributionModel::KatzPoint, "katz_point", {"r_min_m", "d_min_Gy"}};
const RadialDoseModelSpec GEISS{RadialDoseDistributionModel::Geiss, "geiss", {"a0_m"}};
const RadialDoseModelSpec KATZ_SITE{RadialDoseDistributionModel::KatzSite, "katz_site", {"a0_m", "d_min_Gy"}};
const RadialDoseModelSpec CUCINOTTA_POINT{RadialDoseDistributionModel::CucinottaPoint, "cucinotta_point",
                                          {"r_min_m", "d_min_Gy"}};
const RadialDoseModelSpec KATZ_EXTENDED_TARGET{RadialDoseDistributionModel::KatzExtendedTarget,
                                               "katz_extended_target",
                                               {"r_min_m", "a0_m", "d_min_Gy"}};
const RadialDoseModelSpec CUCINOTTA_EXTENDED_TARGET{RadialDoseDistributionModel::CucinottaExtendedTarget,
                                                    "cucinotta_extended_target",
                                                    {"r_min_m", "a0_m", "d_min_Gy"}};

std::string electron_range_model_name(long electron_range_model) {
  for (const auto& [name, model] : ELECTRON_RANGE_MODELS) {
    if (static_cast<long>(model) == electron_range_model) {
      return name;
    }
  }
  return std::to_string(electron_range_model);
}

long parse_electron_range_model(const nb::object& model) {
  if (PyBool_Check(model.ptr())) {
    throw nb::type_error("electron_range_model must be an ElectronRangeModel, string, or integer (not bool)");
  }

  if (nb::isinstance<ElectronRangeModel>(model)) {
    return static_cast<long>(nb::cast<ElectronRangeModel>(model));
  }

  if (nb::isinstance<nb::str>(model)) {
    const auto name = nb::cast<std::string>(model);
    const auto it = ELECTRON_RANGE_MODELS.find(name);
    if (it == ELECTRON_RANGE_MODELS.end()) {
      throw std::invalid_argument("Unknown electron range model name: " + name);
    }
    return static_cast<long>(it->second);
  }

  if (nb::isinstance<nb::int_>(model)) {
    const long model_id = nb::cast<long>(model);
    for (const auto& [name, registered_model] : ELECTRON_RANGE_MODELS) {
      if (static_cast<long>(registered_model) == model_id) {
        return model_id;
      }
    }
    throw std::invalid_argument("Invalid electron range model ID: " + std::to_string(model_id));
  }

  throw nb::type_error("electron_range_model must be an ElectronRangeModel, string, or integer");
}

long parse_compatible_electron_range_model(const RadialDoseModelSpec& spec, const nb::object& model) {
  const long electron_range_model = parse_electron_range_model(model);
  if (is_electron_range_model_compatible(spec.model, electron_range_model)) {
    return electron_range_model;
  }

  std::string compatible_names;
  for (const auto& [name, registered_model] : ELECTRON_RANGE_MODELS) {
    if (is_electron_range_model_compatible(spec.model, static_cast<long>(registered_model))) {
      compatible_names += (compatible_names.empty() ? "'" : ", '") + name + "'";
    }
  }
  throw std::invalid_argument(std::string(spec.function_name) + " is incompatible with electron_range_model '" +
                              electron_range_model_name(electron_range_model) + "'; use one of: " +
                              compatible_names);
}

double parse_real_number(const nb::handle& value, const std::string& argument_name) {
  if (PyBool_Check(value.ptr()) || !(nb::isinstance<nb::float_>(value) || nb::isinstance<nb::int_>(value))) {
    throw nb::type_error((argument_name + " values must be int or float").c_str());
  }
  return nb::cast<double>(value);
}

struct RadiusArgument {
  std::vector<double> values;
  bool is_scalar;
};

RadiusArgument parse_radius_argument(const nb::object& radius_m) {
  RadiusArgument radius{{}, false};

  if (nb::isinstance<nb::ndarray<>>(radius_m)) {
    const auto array = nb::cast<nb::ndarray<>>(radius_m);
    if (array.ndim() > 1) {
      throw std::invalid_argument("radius_m NumPy array must be 0-D or 1-D");
    }
    return parse_radius_argument(radius_m.attr("tolist")());
  }

  if (nb::isinstance<nb::list>(radius_m)) {
    for (const auto value : nb::cast<nb::list>(radius_m)) {
      radius.values.push_back(parse_real_number(value, "radius_m"));
    }
  } else {
    radius.values.push_back(parse_real_number(radius_m, "radius_m"));
    radius.is_scalar = true;
  }

  for (const double value : radius.values) {
    if (!std::isfinite(value) || value < 0.0) {
      throw std::invalid_argument("radius_m must be non-negative and finite");
    }
  }
  return radius;
}

std::array<double, RDD_MAX_NUMBER_OF_PARAMETERS> parse_rdd_parameters(const RadialDoseModelSpec& spec,
                                                                     const nb::object& rdd_parameters) {
  const long rdd_number = static_cast<long>(spec.model);
  const long rdd_index = AT_RDD_index_from_RDD_number(rdd_number);
  if (rdd_index < 0) {
    throw std::runtime_error("Internal error: unknown RDD model number " + std::to_string(rdd_number));
  }

  std::array<double, RDD_MAX_NUMBER_OF_PARAMETERS> parameters{};
  for (size_t i = 0; i < spec.parameter_names.size(); ++i) {
    parameters[i] = AT_RDD_Data.parameter_default[rdd_index][i];
  }

  if (!rdd_parameters.is_none()) {
    if (!nb::isinstance<nb::dict>(rdd_parameters)) {
      throw nb::type_error("rdd_parameters must be a dict or None");
    }

    for (const auto [key, value] : nb::cast<nb::dict>(rdd_parameters)) {
      if (!nb::isinstance<nb::str>(key)) {
        throw nb::type_error("rdd_parameters keys must be strings");
      }
      const auto name = nb::cast<std::string>(key);

      size_t slot = 0;
      while (slot < spec.parameter_names.size() && spec.parameter_names[slot] != name) {
        ++slot;
      }
      if (slot == spec.parameter_names.size()) {
        std::string expected_names;
        for (const auto& parameter_name : spec.parameter_names) {
          expected_names += (expected_names.empty() ? "'" : ", '") + parameter_name + "'";
        }
        throw std::invalid_argument("Unknown rdd_parameters key '" + name + "' for " + spec.function_name +
                                    "; expected one of: " + expected_names);
      }
      parameters[slot] = parse_real_number(value, "rdd_parameters");
    }
  }

  for (size_t i = 0; i < spec.parameter_names.size(); ++i) {
    const auto& name = spec.parameter_names[i];
    const double value = parameters[i];
    if (!std::isfinite(value)) {
      throw std::invalid_argument("rdd_parameters['" + name + "'] must be finite");
    }
    if (name == "d_min_Gy" ? value < 0.0 : value <= 0.0) {
      throw std::invalid_argument("rdd_parameters['" + name + "'] must be " +
                                  (name == "d_min_Gy" ? "non-negative" : "positive"));
    }
  }
  return parameters;
}

struct RadialDoseConfiguration {
  double energy_MeV_u;
  long particle_no;
  long material_no;
};

void evaluate_radial_dose_configuration(const RadialDoseModelSpec& spec, const RadialDoseConfiguration& configuration,
                                        const std::vector<double>& radius_m,
                                        const std::array<double, RDD_MAX_NUMBER_OF_PARAMETERS>& rdd_parameters,
                                        long electron_range_model, long requested_source, bool allow_bethe_fallback,
                                        double* dose_Gy) {
  double energy = configuration.energy_MeV_u;
  long particle_no = configuration.particle_no;
  const long material_no = configuration.material_no;

  if (!std::isfinite(energy) || energy <= 0.0 || energy >= MAXIMUM_ENERGY_MeV_u) {
    throw std::invalid_argument("energy_MeV_u must be finite and in the range (0, 1e6) MeV/u");
  }

  const long source = resolve_stopping_power_source(requested_source, energy, material_no, particle_no);
  if (source == static_cast<long>(StoppingPowerSource::Bethe) &&
      requested_source != static_cast<long>(StoppingPowerSource::Bethe) && !allow_bethe_fallback) {
    throw std::invalid_argument(
        "Bethe fallback is not allowed, but the selected source has no tabular data for the material and energy");
  }
  validate_stopping_power_energy(energy, source, material_no, particle_no);

  // AT_D_RDD_Gy ignores the stopping-power status, so it is checked here first.
  double mass_stopping_power = 0.0;
  const int stopping_status =
      AT_Mass_Stopping_Power_with_no(source, 1, &energy, &particle_no, material_no, &mass_stopping_power);
  if (stopping_status != AT_Success || !std::isfinite(mass_stopping_power) || mass_stopping_power <= 0.0) {
    throw std::invalid_argument("Stopping-power calculation failed for the selected source and material");
  }

  const int status =
      AT_D_RDD_Gy(static_cast<long>(radius_m.size()), radius_m.data(), energy, particle_no, material_no,
                  static_cast<long>(spec.model), rdd_parameters.data(), electron_range_model, source, dose_Gy);
  if (status != AT_Success) {
    throw std::invalid_argument(std::string(spec.function_name) + " calculation failed with libamtrack status " +
                                std::to_string(status));
  }

  for (size_t i = 0; i < radius_m.size(); ++i) {
    if (!std::isfinite(dose_Gy[i]) || dose_Gy[i] < 0.0) {
      throw std::runtime_error(std::string(spec.function_name) + " produced an invalid dose value");
    }
  }
}

/**
 * Shared implementation of all model-specific radial dose functions.
 *
 * energy/particle/material follow the usual elementwise or Cartesian-product semantics; the radial dose of each
 * resulting configuration is evaluated at every radius, which forms the last output axis.
 */
nb::object evaluate_radial_dose(const RadialDoseModelSpec& spec, const nb::object& radius_m,
                                const nb::object& energy_MeV_u, const nb::object& particle,
                                const nb::object& material, const nb::object& rdd_parameters,
                                const nb::object& electron_range_model, const nb::object& source,
                                bool cartesian_product, bool allow_bethe_fallback) {
  validate_particle_argument(particle);
  validate_material_argument(material);

  const RadiusArgument radius = parse_radius_argument(radius_m);
  const auto parameters = parse_rdd_parameters(spec, rdd_parameters);
  const long electron_range_model_no = parse_compatible_electron_range_model(spec, electron_range_model);
  const long requested_source = parse_stopping_power_source(source);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV_u);
  arguments_vector.push_back(parse_particle_argument(particle));
  arguments_vector.push_back(parse_material_argument(material));

  // The generic wrappers only define the configuration layout; the dose itself is evaluated afterwards so
  // validation errors are raised directly instead of being rewrapped by the wrapper.
  std::vector<RadialDoseConfiguration> configurations;
  MultiargumentFunc collect_configuration = [&configurations](const std::vector<std::variant<double, int>>& values) {
    configurations.push_back(
        {variant_cast<double>(values[0]), variant_cast<long>(values[1]), variant_cast<long>(values[2])});
    return static_cast<double>(configurations.size() - 1);
  };

  const nb::object layout = cartesian_product
                                ? wrap_cartesian_product_function(collect_configuration, arguments_vector)
                                : wrap_multiargument_function(collect_configuration, arguments_vector);

  std::vector<size_t> shape;
  if (nb::isinstance<nb::ndarray<>>(layout)) {
    const auto layout_array = nb::cast<nb::ndarray<>>(layout);
    for (size_t i = 0; i < layout_array.ndim(); ++i) {
      shape.push_back(layout_array.shape(i));
    }
  }
  if (!radius.is_scalar) {
    shape.push_back(radius.values.size());
  }

  const size_t n_radii = radius.values.size();
  auto* dose_Gy = new double[configurations.size() * n_radii];
  try {
    for (size_t i = 0; i < configurations.size(); ++i) {
      evaluate_radial_dose_configuration(spec, configurations[i], radius.values, parameters,
                                         electron_range_model_no, requested_source, allow_bethe_fallback,
                                         dose_Gy + i * n_radii);
    }
  } catch (...) {
    delete[] dose_Gy;
    throw;
  }

  if (shape.empty()) {
    const double result = dose_Gy[0];
    delete[] dose_Gy;
    return nb::cast(result);
  }

  nb::capsule owner(dose_Gy, [](void* pointer) noexcept { delete[] static_cast<double*>(pointer); });
  return nb::ndarray<double, nb::numpy>(dose_Gy, shape.size(), shape.data(), owner).cast();
}

}  // namespace

nb::object track_width(const nb::object& energy_MeV, const nb::object& material, const nb::object& model,
                       bool cartesian_product) {
  validate_material_argument(material);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(nb::cast(parse_electron_range_model(model)));

  auto scalar_function = [](const std::vector<std::variant<double, int>>& values) -> double {
    double energy = variant_cast<double>(values[0]);
    int material = variant_cast<int>(values[1]);
    int model = variant_cast<int>(values[2]);

    if (!std::isfinite(energy) || energy <= 0.0 || energy >= MAXIMUM_ENERGY_MeV_u) {
      throw std::invalid_argument("energy_MeV must be finite and in the range (0, 1e6)");
    }

    return AT_max_electron_range_m(energy, material, model);
  };

  if (cartesian_product) {
    return wrap_cartesian_product_function(scalar_function, arguments_vector);
  } else {
    return wrap_multiargument_function(scalar_function, arguments_vector);
  }
}

#define DEFINE_RADIAL_DOSE_FUNCTION(function_name, spec)                                                          \
  nb::object function_name(const nb::object& radius_m, const nb::object& energy_MeV_u, const nb::object& particle,  \
                           const nb::object& material, const nb::object& rdd_parameters,                           \
                           const nb::object& electron_range_model, const nb::object& source,                       \
                           bool cartesian_product, bool allow_bethe_fallback) {                                    \
    return evaluate_radial_dose(spec, radius_m, energy_MeV_u, particle, material, rdd_parameters,                  \
                                electron_range_model, source, cartesian_product, allow_bethe_fallback);            \
  }

DEFINE_RADIAL_DOSE_FUNCTION(katz_point, KATZ_POINT)
DEFINE_RADIAL_DOSE_FUNCTION(geiss, GEISS)
DEFINE_RADIAL_DOSE_FUNCTION(katz_site, KATZ_SITE)
DEFINE_RADIAL_DOSE_FUNCTION(cucinotta_point, CUCINOTTA_POINT)
DEFINE_RADIAL_DOSE_FUNCTION(katz_extended_target, KATZ_EXTENDED_TARGET)
DEFINE_RADIAL_DOSE_FUNCTION(cucinotta_extended_target, CUCINOTTA_EXTENDED_TARGET)

#undef DEFINE_RADIAL_DOSE_FUNCTION
