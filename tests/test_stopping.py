import numpy as np
import pytest

import pyamtrack.materials
import pyamtrack.particles
import pyamtrack.stopping
from pyamtrack.stopping import StoppingPowerSource


@pytest.fixture
def electron_energy_MeV():
    """Fixture providing the electron energy in MeV for tests."""
    return 1000.0


def test_electron_range(electron_energy_MeV):
    """Test the electron_range function for various inputs."""
    range_m = pyamtrack.stopping.electron_range(electron_energy_MeV)
    assert range_m > 0.01, "Expected positive range for positive energy."


def test_electron_range_air_vs_water(electron_energy_MeV):
    """Test the electron_range function for air and water."""
    range_air = pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.air)
    range_water = pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.water_liquid)
    assert range_air > range_water, "Expected range in air to be larger than in water."


def test_material_assignment(electron_energy_MeV):
    """Test the material assignment by ID."""
    range_default = pyamtrack.stopping.electron_range(electron_energy_MeV)
    range_material_name = pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.water_liquid)
    assert range_default == range_material_name, "Expected range to be the same for default and material name."
    range_material_id = pyamtrack.stopping.electron_range(electron_energy_MeV, 1)
    assert range_default == range_material_id, "Expected range to be the same for default and material ID."


def test_mixed_parameter_types(electron_energy_MeV):
    """Test passing each parameter as list or numpy.ndarray, check for output type and shape"""
    range_material_in_array = pyamtrack.stopping.electron_range(energy_MeV=[electron_energy_MeV])
    range_many_materials = pyamtrack.stopping.electron_range(electron_energy_MeV, [1, 2], 3)
    range_many_methods = pyamtrack.stopping.electron_range(
        electron_energy_MeV, pyamtrack.materials.water_liquid, [1, 2]
    )
    range_many_materials_and_methods = pyamtrack.stopping.electron_range(
        electron_energy_MeV, [0, 1, pyamtrack.materials.water_liquid], [3, 4, "tabata"]
    )
    assert isinstance(range_material_in_array, np.ndarray) and range_material_in_array.shape == (1,)
    assert isinstance(range_many_materials, np.ndarray) and range_many_materials.shape == (2,)
    assert isinstance(range_many_methods, np.ndarray) and range_many_methods.shape == (2,)
    assert isinstance(range_many_materials_and_methods, np.ndarray) and range_many_materials_and_methods.shape == (3,)


@pytest.mark.parametrize(
    "dtype1, dtype2",
    [
        (
            np.int64,
            np.int32,
        ),
        (
            np.int16,
            np.uint64,
        ),
        (
            np.int8,
            np.int16,
        ),
        (
            np.uint64,
            int,
        ),
        (np.uint32, np.int_),
        (
            np.short,
            np.int16,
        ),
    ],
)
def test_arrays_with_mixed_dtypes(dtype1: type, dtype2: type):
    """test passing numpy ndarray arguments of different integral dtypes."""
    range_numpy_arrays = pyamtrack.stopping.electron_range(
        [1000, 2000, 3000],
        np.array([0, 1, 2], dtype=dtype1),
        np.array([3, 4, 3], dtype=dtype2),
    )
    assert isinstance(range_numpy_arrays, np.ndarray) and range_numpy_arrays.shape == (3,)


def test_material_assignment_invalid(electron_energy_MeV):
    """Test the material assignment with an invalid ID."""
    with pytest.raises(TypeError):
        pyamtrack.stopping.electron_range(electron_energy_MeV, True)

    with pytest.raises(
        RuntimeError,
        match="Material argument must be an integer or a pyamtrack.materials.Material object",
    ):
        pyamtrack.stopping.electron_range(electron_energy_MeV, "aaa")  # Invalid ID
    with pytest.raises(
        RuntimeError,
        match="Material argument must be an integer or a pyamtrack.materials.Material object",
    ):
        pyamtrack.stopping.electron_range(electron_energy_MeV, pyamtrack.materials.get_ids)


@pytest.mark.skip
def test_invalid_id(electron_energy_MeV):
    """Test the electron_range function with an invalid ID."""
    with pytest.raises(ValueError, match="Invalid material ID"):
        pyamtrack.stopping.electron_range(electron_energy_MeV, 1000000)


PROTON_NO = 1001
CARBON_NO = 6012
WATER_LIQUID_ID = 1
ENERGY_MEV_U = 100.0


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_returns_positive_scalar(func):
    value = func(ENERGY_MEV_U)

    assert isinstance(value, float)
    assert np.isfinite(value)
    assert value > 0.0


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_accepts_ion_and_material_objects(func):
    proton = pyamtrack.particles.proton
    water = pyamtrack.materials.water_liquid

    from_objects = func(ENERGY_MEV_U, particle=proton, material=water)
    from_ids = func(ENERGY_MEV_U, particle=PROTON_NO, material=WATER_LIQUID_ID)

    assert from_objects == pytest.approx(from_ids)


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_carbon_exceeds_proton(func):
    proton = func(ENERGY_MEV_U, particle=pyamtrack.particles.proton)
    carbon = func(ENERGY_MEV_U, particle=pyamtrack.particles.from_string("12C"))

    assert carbon > proton


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_vectorized_energy_shape_and_positivity(func):
    energies = np.array([1.0, 10.0, 100.0], dtype=float)
    values = func(energies, particle=PROTON_NO, material=WATER_LIQUID_ID)

    assert isinstance(values, np.ndarray)
    assert values.shape == energies.shape
    assert np.all(values > 0.0)


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_vectorized_particle_and_material_lists(func):
    particle_values = func(ENERGY_MEV_U, particle=[PROTON_NO, CARBON_NO])
    material_values = func(ENERGY_MEV_U, material=[1, 2])

    assert particle_values.shape == (2,)
    assert material_values.shape == (2,)
    assert np.all(particle_values > 0.0)
    assert np.all(material_values > 0.0)


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_supports_string_and_enum_source(func):
    default_from_string = func(ENERGY_MEV_U, source="default")
    default_from_uppercase = func(ENERGY_MEV_U, source="DEFAULT")
    default_from_enum = func(ENERGY_MEV_U, source=StoppingPowerSource.DEFAULT)
    pstar_from_string = func(ENERGY_MEV_U, source="pstar")
    pstar_from_enum = func(ENERGY_MEV_U, source=StoppingPowerSource.PSTAR)
    bethe_from_string = func(ENERGY_MEV_U, source="bethe")
    icru_from_string = func(ENERGY_MEV_U, source="icru")

    assert default_from_string == pytest.approx(default_from_enum)
    assert default_from_uppercase == pytest.approx(default_from_string)
    assert pstar_from_string == pytest.approx(pstar_from_enum)
    assert default_from_string == pytest.approx(pstar_from_string)
    assert bethe_from_string > 0.0
    assert icru_from_string > 0.0


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_default_source_falls_back_to_bethe_without_pstar(func):
    lead_id = 24
    default_value = func(ENERGY_MEV_U, material=lead_id)
    bethe_value = func(ENERGY_MEV_U, material=lead_id, source="bethe")

    assert default_value == pytest.approx(bethe_value)
    with pytest.raises(ValueError, match="PSTAR"):
        func(ENERGY_MEV_U, material=lead_id, source="pstar")
    with pytest.raises(ValueError, match="ICRU"):
        func(ENERGY_MEV_U, material=lead_id, source="icru")


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_cartesian_product_matches_scalar_calls(func):
    energies = np.array([10.0, 100.0])
    particles = [PROTON_NO, CARBON_NO]

    result = func(energies, particle=particles, cartesian_product=True)
    expected = np.array([[func(energy, particle=particle) for particle in particles] for energy in energies])

    assert result.shape == (len(energies), len(particles))
    np.testing.assert_allclose(result, expected)


@pytest.mark.parametrize("func", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_rejects_invalid_arguments(func):
    with pytest.raises(ValueError):
        func(0.0)

    with pytest.raises(ValueError):
        func(ENERGY_MEV_U, source="unknown")

    with pytest.raises(TypeError):
        func(ENERGY_MEV_U, source=1)

    with pytest.raises(TypeError):
        func(ENERGY_MEV_U, particle="proton")

    with pytest.raises(TypeError):
        func(ENERGY_MEV_U, particle=True)

    with pytest.raises(TypeError):
        func(ENERGY_MEV_U, particle=pyamtrack.particles.from_string("neutron"))

    with pytest.raises(TypeError):
        func(ENERGY_MEV_U, material="water_liquid")

    with pytest.raises(ValueError):
        func(ENERGY_MEV_U, material=0)

    with pytest.raises(ValueError, match="Incompatible lists/arrays size"):
        func([10.0, 100.0], particle=[PROTON_NO])
