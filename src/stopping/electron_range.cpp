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
int process_material(const nb::object& material){
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
    return material_id;
}

int process_model(const nb::object& model){
    int model_id = 0;
    if (nb::isinstance<nb::str>(model)) {
        auto model_name = nb::cast<std::string>(model);
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
    return model_id;
}
bool check_int_dtype(const nb::object& array){
    if(nb::isinstance<nb::ndarray<const int>>(array)
    || nb::isinstance<nb::ndarray<const u_int>>(array)
    || nb::isinstance<nb::ndarray<const long>>(array)
    || nb::isinstance<nb::ndarray<const u_long>>(array)
    ){
        return true;
    }
    return false;
}

nb::object electron_range(const nb::object& input, const nb::object& material, const nb::object& model) {
    std::vector<nb::object> arguments_vector;
    arguments_vector.push_back(input);
    auto electron_range_vector = [](std::vector<double> vec) -> double {
        return AT_max_electron_range_m(vec[0], (int)vec[1], (int)vec[2]);
    };
    if(nb::isinstance<nb::list>(material)){
        auto list = nb::cast<nb::list>(material);
        nb::list material_id;

        for(int i = 0; i< nb::len(list); i++){
            material_id.append(process_material(nb::cast(list[i])));
        }
        arguments_vector.push_back(nb::cast(material_id));
    }
    else if(check_int_dtype(material)){
        arguments_vector.push_back(material);
    }
    else if(nb::isinstance<nb::ndarray<>>(material)){
        throw nb::type_error("material arrays of type other than int unsupported");
    }
    else{
        int material_id = process_material(material);
        arguments_vector.push_back(nb::cast(material_id));
    }

    if(nb::isinstance<nb::list>(model)){
        auto list = nb::cast<nb::list>(model);
        nb::list model_id;
        for(int i = 0; i< nb::len(list); i++){
            model_id.append(process_model(nb::cast(list[i])));
        }
        arguments_vector.push_back(nb::cast(model_id));
    }
    else if(check_int_dtype(model)){
        arguments_vector.push_back(model);
    }
    else if(nb::isinstance<nb::ndarray<>>(model)){
        throw nb::type_error("model arrays of type other than int unsupported");
    }
    else{
        int model_id = process_model(model);
        arguments_vector.push_back(nb::cast(model_id));
    }
    return wrap_multiargument_function(electron_range_vector, arguments_vector);
}