#include "mass_stopping_power.h"

#include <functional>  // For std::function

#include "../materials/materials.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_StoppingPower.h"
}

using ids_getter = std::function<int(const nb::object&)>;

// Forward declaration
static nb::object get_id(const nb::object& object, const ids_getter& getter);

// Reuse process_material from materials.h (already available)
// For particle: accept int or Particle object, extract particle_no
static int process_particle(const nb::object& particle) {
  if (nb::isinstance<nb::int_>(particle)) {
    return nb::cast<int>(particle);
  }
  // Try to treat as a Particle object with a particle_no-style encoding
  try {
    nb::module_ pyamtrack_mod = nb::module_::import_("pyamtrack.particles");
    nb::object ParticleType = pyamtrack_mod.attr("Particle");
    if (nb::isinstance(particle, ParticleType)) {
      int Z = nb::cast<int>(particle.attr("Z"));
      int A = nb::cast<int>(particle.attr("A"));
      return 1000 * Z + A;
    }
  } catch (...) {}
  throw nb::type_error("Particle argument must be an integer (particle_no) or a Particle object");
}

nb::object mass_stopping_power(const nb::object& E_MeV_u,
                               const nb::object& particle,
                               const nb::object& material,
                               int source,
                               bool cartesian_product) {
  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(E_MeV_u);
  arguments_vector.push_back(get_id(particle, process_particle));
  arguments_vector.push_back(get_id(material, process_material));

  // Capture source by value in the lambda
  auto compute = [source](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 3) {
      throw std::invalid_argument("Input vector must have at least three elements.");
    }
    double energy = variant_cast<double>(vec[0]);
    int particle_no = variant_cast<int>(vec[1]);
    int material_no = variant_cast<int>(vec[2]);

    long n = 1;
    long part_no = static_cast<long>(particle_no);
    long mat_no = static_cast<long>(material_no);
    double result = 0.0;

    AT_Mass_Stopping_Power_with_no(
        static_cast<long>(source), n, &energy, &part_no, mat_no, &result);

    return result;
  };

  if (cartesian_product)
    return wrap_cartesian_product_function(compute, arguments_vector);
  return wrap_multiargument_function(compute, arguments_vector);
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