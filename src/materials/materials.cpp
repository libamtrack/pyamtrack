#include "materials.h"
#include <algorithm>
#include <cctype>

Material::Material(long id)
    : id(id) {
    AT_get_material_data(id, &density_g_cm3, &I_eV, &alpha_g_cm2_MeV, &p_MeV, &m_g_cm2, &average_A, &average_Z);
    char material_name[MATERIAL_NAME_LENGTH];
    AT_material_name_from_number(id, material_name);
    name = std::string(material_name);
    phase = AT_phase_from_material_no(id);
}

std::vector<long> get_ids() {
    const AT_table_of_material_data_struct& data = AT_Material_Data;
    return std::vector<long>(std::begin(data.material_no) + 1, std::begin(data.material_no) + data.n);
}

std::vector<std::string> get_long_names() {
    const AT_table_of_material_data_struct& data = AT_Material_Data;
    return std::vector<std::string>(std::begin(data.material_name) + 1, std::begin(data.material_name) + data.n);
}

std::vector<std::string> get_names() {
    auto names = get_long_names();
    // Replace all spaces with underscores
    for (auto& name : names) {
        std::replace(name.begin(), name.end(), ' ', '_');
    }
    // Remove all non-alphanumeric characters
    for (auto& name : names) {
        name.erase(std::remove_if(name.begin(), name.end(), [](char c) { return !std::isalnum(c) && c != '_'; }), name.end());
    }
    return names;
}