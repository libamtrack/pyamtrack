#include "energy_from_energy_per_amu.h"

#include "../particles/particle_arguments.h"
#include "../particles/particles.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object energy_from_energy_per_amu(nb::object energy_MeV_u, nb::object particle, bool cartesian_product) {
  validate_particle_argument(particle);

  std::vector<nb::object> arguments_vector;

  arguments_vector.push_back(energy_MeV_u);
  arguments_vector.push_back(parse_particle_argument(particle));

  auto energy_from_energy_per_amu_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 2) {
      throw std::invalid_argument("Input vector must have at least two elements.");
    };

    double MeV_u = variant_cast<double>(vec[0]);
    long particle_no = variant_cast<long>(vec[1]);

    return AT_E_MeV_from_E_MeV_u(MeV_u, particle_no);
  };

  if (cartesian_product) return wrap_cartesian_product_function(energy_from_energy_per_amu_vector, arguments_vector);
  return wrap_multiargument_function(energy_from_energy_per_amu_vector, arguments_vector);
}
