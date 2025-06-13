import pytest

import pyamtrack


def test_particle_initialization_by_n():
    particle = pyamtrack.particles.Particle(6)  # Carbon
    assert particle.n == 6
    assert particle.element_name == "Carbon"
    assert particle.element_acronym == "C"
    assert particle.Z == 6
    assert particle.atomic_weight > 0
    assert particle.density_g_cm3 > 0
    assert particle.I_eV_per_Z > 0


def test_particle_initialization_by_acronym():
    particle = pyamtrack.particles.Particle("He")  # Helium
    assert particle.n == 2
    assert particle.element_name == "Helium"
    assert particle.element_acronym == "He"
    assert particle.Z == 2
    assert particle.atomic_weight > 0
    assert particle.density_g_cm3 > 0
    assert particle.I_eV_per_Z > 0


def test_particle_initialization_by_invalid_n():
    with pytest.raises(ValueError):
        pyamtrack.particles.Particle(9999)  # Invalid n


def test_particle_initialization_by_invalid_acronym():
    with pytest.raises(ValueError):
        pyamtrack.particles.Particle("Invalid acronym")  # Invalid acronym


def test_get_names():
    names = pyamtrack.particles.get_names()
    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    # assert names[0] == "A"  # Check the first name


def test_get_acronyms():
    names = pyamtrack.particles.get_acronyms()
    assert isinstance(names, list)
    assert len(names) > 0
    assert all(isinstance(name, str) for name in names)
    assert names[0] == "H"  # Check the first name


def test_via_name_object():
    particle = pyamtrack.particles.Carbon
    assert particle.n == 6
    assert particle.element_name == "Carbon"
    assert particle.element_acronym == "C"


def test_via_acronym_object():
    particle = pyamtrack.particles.He
    assert particle.n == 2
    assert particle.element_name == "Helium"
    assert particle.element_acronym == "He"
