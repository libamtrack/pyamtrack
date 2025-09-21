import pytest

import pyamtrack


def test_particle_initialization_by_id():
    particle = pyamtrack.particles.Particle(6)  # Carbon
    assert particle.id == 6
    assert particle.element_name == "Carbon"
    assert particle.element_acronym == "C"
    assert particle.Z == 6
    assert particle.atomic_weight > 0
    assert particle.density_g_cm3 > 0
    assert particle.I_eV_per_Z > 0


def test_particle_initialization_by_acronym():
    particle = pyamtrack.particles.Particle("He")  # Helium
    assert particle.id == 2
    assert particle.element_name == "Helium"
    assert particle.element_acronym == "He"
    assert particle.Z == 2
    assert particle.atomic_weight > 0
    assert particle.density_g_cm3 > 0
    assert particle.I_eV_per_Z > 0


def test_particle_from_number():
    particle = pyamtrack.particles.Particle.from_number(6012)  # Carbon-12
    assert particle.Z == 6
    assert particle.A == 12
    assert particle.element_name == "Carbon"
    assert particle.element_acronym == "C"


def test_particle_initialization_by_id_and_acronym():
    p1 = pyamtrack.particles.Particle.from_number(6012)
    p2 = pyamtrack.particles.Particle.from_number(6012)
    assert p1.Z == p2.Z == 6
    assert p1.A == p2.A == 12
    assert p1.element_name == p2.element_name == "Carbon"


def test_particle_initialization_by_invalid_id():
    with pytest.raises(ValueError):
        pyamtrack.particles.Particle(9999)  # Invalid id


def test_particle_initialization_by_invalid_acronym():
    with pytest.raises(ValueError):
        pyamtrack.particles.Particle("Invalid acronym")  # Invalid acronym


def test_particle_from_number_invalid():
    with pytest.raises(ValueError):
        pyamtrack.particles.Particle.from_number(999999)  # Nonexistent particle


def test_particle_from_string_acronym():
    particle = pyamtrack.particles.Particle.from_string("He")
    assert particle.id == 2
    assert particle.Z == 2
    assert particle.element_name == "Helium"
    assert particle.element_acronym == "He"
    assert particle.A is None  # No mass number parsed


def test_particle_from_string_isotope():
    particle = pyamtrack.particles.Particle.from_string("14C")
    assert particle.id == 6
    assert particle.Z == 6
    assert particle.A == 14
    assert particle.element_name == "Carbon"
    assert particle.element_acronym == "C"


def test_particle_from_string_invalid():
    with pytest.raises(ValueError):
        pyamtrack.particles.Particle.from_string("Xyz123")


def test_get_names():
    names = pyamtrack.particles.get_names()
    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    assert names[0] == "Hydrogen"  # Check the first name


def test_get_acronyms():
    names = pyamtrack.particles.get_acronyms()
    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    assert names[0] == "H"  # Check the first name


def test_via_name_object():
    particle = pyamtrack.particles.Carbon
    assert particle.id == 6
    assert particle.element_name == "Carbon"
    assert particle.element_acronym == "C"


def test_via_acronym_object():
    particle = pyamtrack.particles.He
    assert particle.id == 2
    assert particle.element_name == "Helium"
    assert particle.element_acronym == "He"
