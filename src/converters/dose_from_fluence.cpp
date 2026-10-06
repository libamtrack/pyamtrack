#include "dose_from_fluence.h"

#include "../materials/materials.h"
#include "../particles/particle_arguments.h"
#include "../particles/particles.h"
#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object dose_from_fluence(nb::object energy_MeV, nb::object particle, nb::object fluence_cm2, nb::object material,
                             nb::object stopping_power_source, bool cartesian_product) {
  validate_material_argument(material);
  validate_particle_argument(particle);

  std::vector<nb::object> arguments_vector;

  arguments_vector.push_back(energy_MeV);
  arguments_vector.push_back(parse_particle_argument(particle));
  arguments_vector.push_back(fluence_cm2);
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(stopping_power_source);

  auto dose_from_fluence_vector = [](const std::vector<std::variant<double, int>>& vec) -> double {
    if (vec.size() < 5) {
      throw std::invalid_argument("Input vector must have at least five elements.");
    };

    double energy = variant_cast<double>(vec[0]);
    long particle_no = variant_cast<long>(vec[1]);
    double fluence = variant_cast<double>(vec[2]);
    long material_no = variant_cast<long>(vec[3]);
    long stopping_power_source_no = variant_cast<long>(vec[4]);

    return AT_dose_Gy_from_fluence_cm2_single(AT_E_MeV_u_from_E_MeV(energy, particle_no), particle_no, fluence,
                                              material_no, stopping_power_source_no);
  };

  if (cartesian_product) return wrap_cartesian_product_function(dose_from_fluence_vector, arguments_vector);
  return wrap_multiargument_function(dose_from_fluence_vector, arguments_vector);
}
