#include "../wrapper_template.h" // Should contain wrap_function
#include "electron_range.h"   // May contain STOPPING_MODELS definition?
#include <string>             // For std::string
#include <vector>             // For std::vector
#include <stdexcept>          // For std::runtime_error
#include <map>                // If STOPPING_MODELS is a map

// Assuming STOPPING_MODELS is defined somewhere accessible, like electron_range.h
// Example definition if needed:
// static const std::map<std::string, int> STOPPING_MODELS = {
//    {"ModelName1", 1}, {"ModelName2", 2} /* ... more models ... */
// };
// If it's not a map, adjust the helper functions accordingly.


extern "C" {
    #include "AT_ElectronRange.h" // Contains AT_max_electron_range_m definition
}

// Helper functions using the map (ensure STOPPING_MODELS is defined and accessible)
std::vector<std::string> get_models() {
    std::vector<std::string> names;
    // If STOPPING_MODELS isn't available, this logic needs changing
    // For example, maybe read names from a structure in AT_ElectronRange.h
#ifdef STOPPING_MODELS // Conditional compilation if it might not be defined
    for (const auto& pair : STOPPING_MODELS) { // Use structured binding if C++17
        names.push_back(pair.first);
    }
#else
    // Placeholder if map isn't defined/available
    names = {"Waligorski", "ButtsKatz", "Geiss"}; // Example names
#endif
    return names;
}

int get_model_id(const std::string& model_name) {
#ifdef STOPPING_MODELS
    auto it = STOPPING_MODELS.find(model_name);
    if (it == STOPPING_MODELS.end()) {
        throw std::runtime_error("Unknown model name: " + model_name);
    }
    return it->second;
#else
    // Placeholder if map isn't defined/available
    if (model_name == "Waligorski") return 2; // Example IDs
    if (model_name == "ButtsKatz") return 1;
    if (model_name == "Geiss") return 3;
    throw std::runtime_error("Unknown model name (map unavailable): " + model_name);
#endif
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
            throw nb::type_error(error_msg.c_str()); // nb::type_error is correct here
        } catch (const std::exception &e) { // Catches std::runtime_error and others
            // FIX: Use std::runtime_error from <stdexcept>
            std::string error_msg = "An error occurred while processing the Material object: " + std::string(e.what());
            throw std::runtime_error(error_msg.c_str()); // Nanobind will translate this
        }
    }

    int model_id = 0;
    if (nb::isinstance<nb::str>(model)) {
        std::string model_name = nb::cast<std::string>(model);
         try {
            model_id = get_model_id(model_name);
         } catch (const std::runtime_error &e) { // Catches error from get_model_id
             // Throwing nb::value_error is appropriate here for invalid input name
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