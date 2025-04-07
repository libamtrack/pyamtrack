#include "materials.h"

Material::Material(long id)
    : id(id) {
    AT_get_material_data(id, &density_g_cm3, &I_eV, &alpha_g_cm2_MeV, &p_MeV, &m_g_cm2, &average_A, &average_Z);
    char material_name[MATERIAL_NAME_LENGTH];
    AT_material_name_from_number(id, material_name);
    name = std::string(material_name);
    phase = AT_phase_from_material_no(id);
}