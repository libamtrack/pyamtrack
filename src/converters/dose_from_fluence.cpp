#include "dose_from_fluence.h"

#include "../wrapper/multi_argument.h"
#include "../materials/materials.h"
#include "../particles/particles.h"

extern "C" {
#include "AT_PhysicsRoutines.h"
}

nb::object dose_from_fluence(nb::object energy_MeV_u,
                            nb::object particle,
                            nb::object fluence_cm2,
                            nb::object material,
                            nb::object stopping_power_source) {
 
                                
 validate_material_argument(material);

  std::vector<nb::object> arguments_vector;

  arguments_vector.push_back(energy_MeV_u);
  arguments_vector.push_back(parse_particle_argument(particle));
  arguments_vector.push_back(parse_material_argument(material));
  arguments_vector.push_back(stopping_power_source)

  auto dose_from_fluence_vector: lambda = [](const std::vector<std::variant<double, int>>& vec) -> double {
        if (vec.size() < 4) {
      throw std::invalid_argument("Input vector must have at least six elements.");
    }
  }


  return wrap_function(AT_E_from_gamma_single, gamma);
}