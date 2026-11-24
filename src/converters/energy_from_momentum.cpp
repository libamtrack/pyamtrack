#include "energy_from_momentum.h"

#include "../particles/particle_arguments.h"
#include "../particles/particles.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"
#include "../wrapper/single_argument.h"


extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object energy_from_momentum(nb::object momentum, const nb::object particle, const bool cartesian_product) {
  validate_particle_argument(particle);

  std::vector<nb::object> arguments_vector;
  arguments_vector.push_back(momentum);
  arguments_vector.push_back(parse_particle_argument(particle));

  auto energy_from_momentum_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 2) {
      throw std::invalid_argument("Input vector must have at least two elements.");
    }

    const double momentum_c = variant_cast<double>(vec[0]);
    const long particle_no = static_cast<long>(variant_cast<int>(vec[1]));


    return AT_E_MeV_from_E_MeV_u(AT_E_MeV_u_from_momentum_single(momentum_c), particle_no);
  };

  if (cartesian_product) {
    return wrap_cartesian_product_function(energy_from_momentum_vector, arguments_vector);
  }

  return wrap_multiargument_function(energy_from_momentum_vector, arguments_vector);
}