#include "../wrapper_template.h"
#include "electron_range.h"

extern "C" {
    #include "AT_ElectronRange.h"
}

// Helper functions using the map
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

// Main function to handle different input types
py::object electron_range(py::object input, py::object material, py::object model) {
    int material_id = 0;
    if (py::isinstance<py::int_>(material)) {
        material_id = material.cast<int>();
    } else {
        py::module_ pyamtrack = py::module_::import("pyamtrack");
        py::module_ materials = pyamtrack.attr("materials");
        py::object Material = materials.attr("Material");

        if (!py::isinstance(material, Material)) {
            throw py::type_error("Material argument must be either an integer or a Material object");
        }
        
        try {
            material_id = material.attr("id").cast<int>();
        } catch (const py::error_already_set&) {
            throw py::type_error("Material argument must be either an integer or a Material object");
        }
    }

    int model_id = 0;
    if( py::isinstance<py::str>(model) ) {
        std::string model_name = model.cast<std::string>();
        model_id = get_model_id(model_name);
    } else if (py::isinstance<py::int_>(model)) {
        model_id = model.cast<int>();
    } else {
        throw py::type_error("Model argument must be either an integer or a string");
    }

    // create a lambda function to capture material_id
    auto electron_range_single = [material_id, model_id](double E_MeV_u) {
        return AT_max_electron_range_m(E_MeV_u, material_id, model_id);
    };

    return wrap_function(electron_range_single, input);
}