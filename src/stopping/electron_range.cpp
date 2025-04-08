#include "../wrapper_template.h"
#include "electron_range.h"

extern "C" {
    #include "AT_ElectronRange.h"
}

// Main function to handle different input types
py::object electron_range(py::object input, py::object material = py::int_(1)) {
    int material_id = 0;
    if (py::isinstance<py::int_>(material)) {
        material_id = material.cast<int>();
    } else {
        // Check if material is an instance of Material class
        py::module_ builtins = py::module_::import("builtins");
        py::object type_func = builtins.attr("type");
        py::str type_name = type_func(material).attr("__name__").cast<py::str>();
        
        if (std::string(type_name) != "Material") {
            throw py::type_error("Material argument must be either an integer or a Material object");
        }
        
        try {
            material_id = material.attr("id").cast<int>();
        } catch (const py::error_already_set&) {
            throw py::type_error("Material object must have 'id' attribute");
        }
    }

    // create a lambda function to capture material_id
    auto electron_range_single = [material_id](double E_MeV_u) {
        return AT_max_electron_range_m(E_MeV_u, material_id, 7);
    };

    return wrap_function(electron_range_single, input);
}