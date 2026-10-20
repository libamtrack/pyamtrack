#include "momentum_from_energy.h"

#include "../particles/particle_arguments.h"
#include "../particles/particles.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"
#include "../wrapper/single_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object momentum_from_energy(nb::object energy_MeV, const nb::object particle, const bool cartesian_product) {
  validate_particle_argument(particle);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(parse_particle_argument(particle));

  auto momentum_from_energy_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 2) {
      throw std::invalid_argument("Input vector must have at least two elements.");
    }

    const double energy_MeV = variant_cast<double>(vec[0]);
    const long particle_no = static_cast<long>(variant_cast<int>(vec[1]));

    const double energy_MeV_u = AT_E_MeV_u_from_E_MeV(energy_MeV, particle_no);

    return AT_momentum_from_E_MeV_c_u_single(energy_MeV_u);
  };

  if (cartesian_product) {
    return wrap_cartesian_product_function(momentum_from_energy_vector, arguments_vector);
  }

  return wrap_multiargument_function(momentum_from_energy_vector, arguments_vector);
}