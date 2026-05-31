import pytest

import pyamtrack


particles = pyamtrack.particles


def assert_carbon_12(ion):
    assert isinstance(ion, particles.ions.Ion)
    assert ion.id == 6
    assert ion.Z == 6
    assert ion.A == 12
    assert ion.element_name == "Carbon"
    assert ion.element_acronym == "C"
    assert ion.atomic_weight > 0


def test_from_string_acronym_uses_default_isotope():
    ion = particles.from_string("C")

    assert_carbon_12(ion)


def test_from_string_isotope_prefix_notation():
    ion = particles.from_string("14C")

    assert isinstance(ion, particles.ions.Ion)
    assert ion.id == 6
    assert ion.Z == 6
    assert ion.A == 14
    assert ion.element_name == "Carbon"
    assert ion.element_acronym == "C"


def test_from_string_isotope_suffix_notation():
    ion = particles.from_string("C-12")

    assert_carbon_12(ion)


def test_from_string_invalid_symbol():
    with pytest.raises(ValueError):
        particles.from_string("Xyz123")


def test_from_string_invalid_mass_number():
    with pytest.raises(ValueError):
        particles.from_string("99C")


def test_from_string_elementary_particles():
    neutron = particles.from_string("neutron")
    electron = particles.from_string("electron")

    assert isinstance(neutron, particles.Particle)
    assert not isinstance(neutron, particles.ions.Ion)
    assert neutron.element_name == "neutron"
    assert neutron.element_acronym == "n"

    assert isinstance(electron, particles.Particle)
    assert not isinstance(electron, particles.ions.Ion)
    assert electron.element_name == "electron"
    assert electron.element_acronym == "e"


def test_from_ZA():
    ion = particles.from_ZA(6, 12)

    assert_carbon_12(ion)


def test_from_ZA_invalid_atomic_number():
    with pytest.raises(ValueError):
        particles.from_ZA(999, 12)


def test_from_pdg_ion():
    ion = particles.from_pdg(1000060120)

    assert_carbon_12(ion)


def test_from_pdg_elementary_particles():
    neutron = particles.from_pdg(2112)
    electron = particles.from_pdg(11)

    assert isinstance(neutron, particles.Particle)
    assert not isinstance(neutron, particles.ions.Ion)
    assert neutron.element_name == "neutron"

    assert isinstance(electron, particles.Particle)
    assert not isinstance(electron, particles.ions.Ion)
    assert electron.element_name == "electron"


def test_particle_constructor_accepts_elementary_pdg():
    neutron = particles.Particle(2112)

    assert isinstance(neutron, particles.Particle)
    assert not isinstance(neutron, particles.ions.Ion)
    assert neutron.element_name == "neutron"


def test_particle_constructor_rejects_ion_pdg():
    with pytest.raises(ValueError):
        particles.Particle(1000060120)


def test_ion_constructor_accepts_ion_pdg():
    ion = particles.ions.Ion(1000060120)

    assert_carbon_12(ion)


def test_ion_constructor_rejects_elementary_pdg():
    with pytest.raises(ValueError):
        particles.ions.Ion(2112)


def test_get_names():
    names = particles.get_names()

    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    assert names[0] == "Hydrogen"


def test_get_acronyms():
    acronyms = particles.get_acronyms()

    assert isinstance(acronyms, list)
    assert len(acronyms) > 0
    assert all(isinstance(acronym, str) for acronym in acronyms)
    assert acronyms[0] == "H"


def test_via_acronym_module_attribute():
    ion = particles.He

    assert isinstance(ion, particles.ions.Ion)
    assert ion.id == 2
    assert ion.Z == 2
    assert ion.element_name == "Helium"
    assert ion.element_acronym == "He"


def test_proton_module_attribute():
    proton = particles.proton

    assert isinstance(proton, particles.ions.Ion)
    assert proton.id == 1
    assert proton.Z == 1
    assert proton.A == 1
    assert proton.element_name == "Hydrogen"
    assert proton.element_acronym == "H"
