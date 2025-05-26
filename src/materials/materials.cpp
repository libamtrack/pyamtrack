#include "materials.h"

#include <algorithm>
#include <cctype>

Material::Material(long id) : id(id) {
  auto material_index = AT_material_index_from_material_number(id);
  if (material_index < 0) {
    throw std::invalid_argument("Material not found: " + std::to_string(id));
  }
  AT_get_material_data(id, &density_g_cm3, &I_eV, &alpha_g_cm2_MeV, &p_MeV, &m_g_cm2, &average_A, &average_Z);
  char material_name[MATERIAL_NAME_LENGTH];
  AT_material_name_from_number(id, material_name);
  name = std::string(material_name);
  phase = AT_phase_from_material_no(id);
}

Material::Material(const std::string& name) {
  auto id = AT_material_number_from_name(name.c_str());
  if (id < 1) {
    throw std::invalid_argument("Material not found: " + name);
  }
  AT_get_material_data(id, &density_g_cm3, &I_eV, &alpha_g_cm2_MeV, &p_MeV, &m_g_cm2, &average_A, &average_Z);
  this->id = id;
  phase = AT_phase_from_material_no(id);
  this->name = name;
}

std::vector<long> get_ids() {
  const AT_table_of_material_data_struct& data = AT_Material_Data;
  return std::vector<long>(std::begin(data.material_no) + 1, std::begin(data.material_no) + data.n);
}

std::vector<std::string> get_long_names() {
  const AT_table_of_material_data_struct& data = AT_Material_Data;
  return std::vector<std::string>(std::begin(data.material_name) + 1, std::begin(data.material_name) + data.n);
}

std::string to_name(const std::string& name) {
  std::string result = name;
  // Replace all spaces with underscores
  std::replace(result.begin(), result.end(), ' ', '_');
  // Remove all non-alphanumeric characters
  result.erase(std::remove_if(result.begin(), result.end(), [](char c) { return !std::isalnum(c) && c != '_'; }),
               result.end());
  // Convert to lowercase
  std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
  return result;
}

std::vector<std::string> get_names() {
  auto names = get_long_names();
  for (auto& name : names) {
    name = to_name(name);
  }
  return names;
}
