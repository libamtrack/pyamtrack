#include "ions.h"


Ion from_ZA(long long Z, long long A) {
    const auto& data = AT_Particle_Data;
  for (int i = 0; i < data.n; ++i) {
    if (data.Z[i] == Z) {
      std::string acronym(data.element_acronym[i]);
      Ion p(acronym + std::to_string(A));
      return p;
    }
  }

  throw std::invalid_argument("Particle with Z=" + std::to_string(Z) + " not found");
}