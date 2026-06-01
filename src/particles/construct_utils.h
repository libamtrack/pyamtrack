#pragma once
#include <string>
#include <map>


extern "C" {
  #include "AT_DataParticle.h"
}


/**
 * @brief Parse an isotope string into element symbol and mass number.
 *
 * Accepts element-first (e.g. "C", "C-12", "Carbon_12") or
 * mass-first (e.g. "12C", "12-Carbon") forms. Case-insensitive for
 * element symbols/names.
 *
 * @param isotope The input isotope string.
 * @return std::pair<std::string, int> Pair of (symbol, A). A is -1 when absent.
 * @throws std::invalid_argument If the format is invalid.
 */
extern std::pair<std::string, int> parse_isotope(std::string isotope);

/**
 * @brief Calculate PDG nuclear code from Z and A.
 *
 * Uses the PDG nuclear encoding: $10^9 + L\cdot10^7 + Z\cdot10^4 + A\cdot10 + I$.
 *
 * @param Z Atomic number.
 * @param A Mass number.
 * @param L Lambda count (default 0).
 * @param I Isomer level (default 0).
 * @return long long PDG code.
 */
long long calculatePDG(int Z, int A, int L = 0, int I = 0);

/**
 * @brief Convert a string to lowercase.
 *
 * @param s Input string.
 * @return std::string Lowercased copy.
 */
std::string to_lower_case(const std::string& s);

/**
 * @brief Default mass numbers for elements when A is omitted.
 */
inline std::map<std::string, int> most_popular_iso_A = {
  {"H", 1},    // protium
  {"He", 4},
  {"Li", 7},
  {"Be", 9},
  {"B", 11},
  {"C", 12},
  {"N", 14},
  {"O", 16},
  {"F", 19},
  {"Ne", 20},

  {"Na", 23},
  {"Mg", 24},
  {"Al", 27},
  {"Si", 28},
  {"P", 31},
  {"S", 32},
  {"Cl", 35},  // actually mostly 35 and 37; 35 is more abundant
  {"Ar", 40},

  {"K", 39},
  {"Ca", 40},
  {"Sc", 45},
  {"Ti", 48},
  {"V", 51},
  {"Cr", 52},
  {"Mn", 55},
  {"Fe", 56},
  {"Co", 59},
  {"Ni", 58},
  {"Cu", 63},
  {"Zn", 64},

  {"Ga", 69},
  {"Ge", 74},
  {"As", 75},
  {"Se", 80},
  {"Br", 79},
  {"Kr", 84},

  {"Rb", 85},
  {"Sr", 88},
  {"Y", 89},
  {"Zr", 90},
  {"Nb", 93},
  {"Mo", 96},
  {"Tc", 98},
  {"Ru", 102},
  {"Rh", 103},
  {"Pd", 106},
  {"Ag", 107},
  {"Cd", 114},
  {"In", 115},
  {"Sn", 119},
  {"Sb", 121},
  {"Te", 130},
  {"I", 127},
  {"Xe", 132},

  {"Cs", 133},
  {"Ba", 138},
  {"La", 139},
  {"Ce", 140},
  {"Pr", 141},
  {"Nd", 142},
  {"Pm", 145},
  {"Sm", 152},
  {"Eu", 153},
  {"Gd", 158},
  {"Tb", 159},
  {"Dy", 163},
  {"Ho", 165},
  {"Er", 166},
  {"Tm", 169},
  {"Yb", 174},
  {"Lu", 175},
  {"Hf", 180},
  {"Ta", 181},
  {"W", 184},
  {"Re", 187},
  {"Os", 192},
  {"Ir", 193},
  {"Pt", 195},
  {"Au", 197},
  {"Hg", 202},
  {"Tl", 205},
  {"Pb", 208},
  {"Bi", 209},
  {"Po", 209},
  {"Rn", 222},
  {"Ra", 226},
  {"Ac", 227},
  {"Th", 232},
  {"Pa", 231},
  {"U", 238},
  {"Np", 237},
  {"Pu", 244},
  {"Am", 243},
  {"Cm", 247},
  {"Bk", 247},
  {"Cf", 251}
};

/**
 * @brief Valid mass-number ranges for supported elements.
 */
inline std::map<std::string, std::pair<int, int>> isotope_A_range = {
  {"H",  {1, 3}},
  {"He", {3, 4}},
  {"Li", {6, 7}},
  {"Be", {9, 9}},
  {"B",  {10, 11}},
  {"C",  {12, 14}},
  {"N",  {14, 15}},
  {"O",  {16, 18}},
  {"F",  {19, 19}},
  {"Ne", {20, 22}},

  {"Na", {23, 23}},
  {"Mg", {24, 26}},
  {"Al", {27, 27}},
  {"Si", {28, 30}},
  {"P",  {31, 31}},
  {"S",  {32, 36}},
  {"Cl", {35, 37}},
  {"Ar", {36, 40}},

  {"K",  {39, 41}},
  {"Ca", {40, 48}},
  {"Sc", {45, 45}},
  {"Ti", {46, 50}},
  {"V",  {50, 51}},
  {"Cr", {50, 54}},
  {"Mn", {55, 55}},
  {"Fe", {54, 58}},
  {"Co", {59, 59}},
  {"Ni", {58, 64}},
  {"Cu", {63, 65}},
  {"Zn", {64, 70}},

  {"Ga", {68, 70}},
  {"Ge", {70, 76}},
  {"As", {74, 76}},
  {"Se", {74, 82}},
  {"Br", {77, 81}},
  {"Kr", {78, 86}},

  {"Rb", {85, 87}},
  {"Sr", {84, 88}},
  {"Y",  {89, 89}},
  {"Zr", {90, 96}},
  {"Nb", {93, 95}},
  {"Mo", {92, 100}},
  {"Tc", {97, 99}},
  {"Ru", {96, 104}},
  {"Rh", {103, 105}},
  {"Pd", {102, 110}},
  {"Ag", {107, 109}},
  {"Cd", {106, 116}},
  {"In", {113, 115}},
  {"Sn", {112, 124}},
  {"Sb", {121, 123}},
  {"Te", {120, 130}},
  {"I",  {127, 127}},
  {"Xe", {124, 136}},

  {"Cs", {133, 133}},
  {"Ba", {130, 138}},
  {"La", {138, 139}},
  {"Ce", {136, 142}},
  {"Pr", {141, 141}},
  {"Nd", {142, 144}},
  {"Pm", {145, 145}},
  {"Sm", {144, 154}},
  {"Eu", {151, 153}},
  {"Gd", {152, 160}},
  {"Tb", {159, 159}},
  {"Dy", {156, 164}},
  {"Ho", {165, 165}},
  {"Er", {162, 168}},
  {"Tm", {169, 169}},
  {"Yb", {168, 176}},
  {"Lu", {175, 175}},
  {"Hf", {174, 180}},
  {"Ta", {181, 181}},
  {"W",  {180, 186}},
  {"Re", {185, 187}},
  {"Os", {184, 194}},
  {"Ir", {191, 193}},
  {"Pt", {190, 198}},
  {"Au", {197, 197}},
  {"Hg", {196, 204}},
  {"Tl", {203, 205}},
  {"Pb", {204, 208}},
  {"Bi", {209, 209}},
  {"Po", {209, 210}},
  {"Rn", {222, 222}},
  {"Ra", {226, 226}},
  {"Ac", {227, 227}},
  {"Th", {232, 232}},
  {"Pa", {231, 231}},
  {"U",  {234, 238}},
  {"Np", {237, 237}},
  {"Pu", {238, 244}},
  {"Am", {241, 243}},
  {"Cm", {242, 246}},
  {"Bk", {247, 247}},
  {"Cf", {249, 251}}
};


inline std::map<std::string, long long> pdg_for_nonions = {
  {"neutron", 2122},
  {"electron", 11}
};