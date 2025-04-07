#include "materials.h"

Material::Material(long material_no)
    : material_no(material_no) {
    AT_get_material_data(material_no, &density_g_cm3, &I_eV, &alpha_g_cm2_MeV, &p_MeV, &m_g_cm2, &average_A, &average_Z);
    char name[MATERIAL_NAME_LENGTH];
    AT_material_name_from_number(material_no, name);
    material_name = std::string(name);
    phase = AT_phase_from_material_no(material_no);
}