#include <nanobind/nanobind.h>

#include "../wrapper/cartesian_product.h"
#include "../wrapper/multi_argument.h"
#include "track_radial.h"
#include "../materials/materials.h"

extern "C" {
    #include "AT_RDD.h"
}

nb::object track_width(const nb::object& energy_MeV, const nb::object& material, const nb::object& model, bool cartesian_product) {
    validate_material_argument(material);
    
    std::vector<nb::object> arguments_vector;
    arguments_vector.push_back(energy_MeV);
    arguments_vector.push_back(parse_material_argument(material));
    arguments_vector.push_back(nb::cast<long>(AT_RDD_index_from_RDD_number(static_cast<long>(nb::cast<RadialDoseDistributionModel>(model)))));

    auto scalar_function =
    [](const std::vector<std::variant<double, int>>& values) -> double {
        double energy = variant_cast<double>(values[0]);
        int material = variant_cast<int>(values[1]);
        int model = variant_cast<int>(values[2]);

        return AT_max_electron_range_m(energy, material, model);
    };

    if (cartesian_product) {
        return wrap_cartesian_product_function(scalar_function, arguments_vector);
    } else {
        return wrap_multiargument_function(scalar_function, arguments_vector);
    }
}


