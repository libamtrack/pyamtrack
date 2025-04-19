#include "../wrapper_template.h" // Should contain wrap_function
#include "electron_range.h"
#include <string>             // For std::string
#include <vector>             // For std::vector
#include <stdexcept>          // For std::runtime_error


extern "C" {
    #include "AT_ElectronRange.h" // Contains AT_max_electron_range_m definition
}

std::vector<std::string> get_models() {
    std::vector<std::string> names;
    for (const auto& [name, id] : STOPPING_MODELS) {
        names.push_back(name);
    }
    return names;
}

int get_model_id(const std::string& model_name) {
    auto it = STOPPING_MODELS.find(model_name);
    if (it == STOPPING_MODELS.end()) {
        throw std::runtime_error("Unknown model name: " + model_name);
    }
    return it->second;
}


nb::object electron_range(nb::object input, nb::object material, nb::object model) {
    int material_id = 0;
    if (nb::isinstance<nb::int_>(material)) {
        material_id = nb::cast<int>(material);
    } else {
        try {
            nb::module_ pyamtrack_mod = nb::module_::import_("pyamtrack.materials");
            nb::object MaterialType = pyamtrack_mod.attr("Material");

            if (!nb::isinstance(material, MaterialType)) {
                 throw nb::type_error("Material argument must be an integer or a pyamtrack.materials.Material object");
            }

            material_id = nb::cast<int>(material.attr("id"));

        } catch (const nb::python_error &e) {
            // FIX: Re-throw the original Python error
            throw; // Preserves original Python exception type and traceback
        } catch (const nb::cast_error &e) {
            std::string error_msg = "Material object's 'id' attribute is not an integer: " + std::string(e.what());
            throw nb::type_error(error_msg.c_str());
        } catch (const std::exception &e) { // Catches std::runtime_error and others
            std::string error_msg = "An error occurred while processing the Material object: " + std::string(e.what());
            throw std::runtime_error(error_msg.c_str()); // Nanobind will translate this
        }
    }

    int model_id = 0;
    if (nb::isinstance<nb::str>(model)) {
        std::string model_name = nb::cast<std::string>(model);
         try {
            model_id = get_model_id(model_name);
         } catch (const std::runtime_error &e) {
             throw nb::value_error(e.what());
         }
    } else if (nb::isinstance<nb::int_>(model)) {
        model_id = nb::cast<int>(model);
    } else {
        throw nb::type_error("Model argument must be either an integer or a string");
    }

    auto electron_range_single = [material_id, model_id](double e_mev_u) -> double {
        return AT_max_electron_range_m(e_mev_u, material_id, model_id);
    };

    return wrap_function(electron_range_single, input);
}