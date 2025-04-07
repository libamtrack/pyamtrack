#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "materials.h"
#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(materials, m) {
    m.doc() = "Functions and data structures for accessing and manipulating material properties.";

    py::class_<Material>(m, "Material")
        .def(py::init<long>())
        .def_readonly("id", &Material::id)
        .def_readonly("density_g_cm3", &Material::density_g_cm3)
        .def_readonly("I_eV", &Material::I_eV)
        .def_readonly("alpha_g_cm2_MeV", &Material::alpha_g_cm2_MeV)
        .def_readonly("p_MeV", &Material::p_MeV)
        .def_readonly("m_g_cm2", &Material::m_g_cm2)
        .def_readonly("average_A", &Material::average_A)
        .def_readonly("average_Z", &Material::average_Z)
        .def_readonly("name", &Material::name)
        .def_readonly("phase", &Material::phase);

    m.def("get_ids", []() {
        const AT_table_of_material_data_struct& data = AT_Material_Data;
        return std::vector<long>(std::begin(data.material_no) + 1, std::begin(data.material_no) + data.n);
    });

    m.def("get_long_names", []() {
        const AT_table_of_material_data_struct& data = AT_Material_Data;
        return std::vector<std::string>(std::begin(data.material_name) + 1, std::begin(data.material_name) + data.n);
    });

    m.def("get_names", []() {
        const AT_table_of_material_data_struct& data = AT_Material_Data;
        auto names = std::vector<std::string>(std::begin(data.material_name) + 1, std::begin(data.material_name) + data.n);
        // replace all spaces with underscores
        for (auto& name : names) {
            std::replace(name.begin(), name.end(), ' ', '_');
        }
        // remove all non-alphanumeric characters
        for (auto& name : names) {
            name.erase(std::remove_if(name.begin(), name.end(), [](char c) { return !std::isalnum(c) && c != '_'; }), name.end());
        }
        return names;
    });

}