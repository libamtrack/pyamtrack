#include "../wrapper_template.h"
#include "max_E_transfer_MeV.h"
#include <vector>
#include <functional>
extern "C" {
#include "AT_PhysicsRoutines.h"
}

double vector_function(std::vector<double> vec){
    return AT_max_E_transfer_MeV_new_single(vec[0], vec[1]);
}

nb::object max_E_transfer_MeV(nb::object E_MeV_u, nb::object A){
    std::vector<nb::object> args_vec;
    args_vec.push_back(E_MeV_u);
    args_vec.push_back(A);
    return wrap_multiargument_function(vector_function, args_vec);
}
