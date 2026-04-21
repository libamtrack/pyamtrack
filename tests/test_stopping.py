import numpy as np
import pytest

import pyamtrack.materials
import pyamtrack.particles
import pyamtrack.stopping


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


# ──────────────────────────────────────────────────────────────────────────────
# Tests for mass_stopping_power
# ──────────────────────────────────────────────────────────────────────────────

PROTON_NO = 1001  # 1000*Z + A for proton
CARBON_NO = 6012  # 1000*Z + A for carbon-12
WATER_LIQUID_ID = 1


@pytest.fixture
def proton_energy_MeV_u():
    """Typical proton energy in MeV/u for stopping-power tests."""
    return 100.0


def test_mass_stopping_power_scalar_positive(proton_energy_MeV_u):
    """mass_stopping_power returns a positive float for a scalar proton energy."""
    result = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u)
    assert isinstance(result, float)
    assert result > 0, "Mass stopping power must be positive"


def test_mass_stopping_power_list_input(proton_energy_MeV_u):
    """mass_stopping_power accepts a list and returns an ndarray of the same length."""
    energies = [proton_energy_MeV_u, 10.0, 1.0]
    result = pyamtrack.stopping.mass_stopping_power(energies)
    assert isinstance(result, np.ndarray)
    assert result.shape == (3,)
    assert np.all(result > 0), "All mass stopping power values must be positive"


def test_mass_stopping_power_numpy_input():
    """mass_stopping_power accepts a numpy array and returns an ndarray of the same shape."""
    energies = np.logspace(np.log10(0.01), np.log10(500), 20)
    result = pyamtrack.stopping.mass_stopping_power(energies)
    assert isinstance(result, np.ndarray)
    assert result.shape == energies.shape
    assert np.all(result > 0), "All mass stopping power values must be positive"


def test_mass_stopping_power_particle_object(proton_energy_MeV_u):
    """mass_stopping_power accepts a Particle object constructed via from_number."""
    particle = pyamtrack.particles.Particle.from_number(PROTON_NO)
    result_obj = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, particle=particle)
    result_int = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, particle=PROTON_NO)
    assert result_obj == pytest.approx(result_int)


def test_mass_stopping_power_material_object(proton_energy_MeV_u):
    """mass_stopping_power accepts a Material object."""
    material = pyamtrack.materials.water_liquid
    result_obj = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, material=material)
    result_int = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, material=WATER_LIQUID_ID)
    assert result_obj == pytest.approx(result_int)


def test_mass_stopping_power_carbon(proton_energy_MeV_u):
    """mass_stopping_power returns a higher value for carbon than proton at same energy/nucleon."""
    msp_proton = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, particle=PROTON_NO)
    msp_carbon = pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, particle=CARBON_NO)
    assert msp_carbon > msp_proton, "Carbon should have higher mass stopping power than proton"


def test_mass_stopping_power_invalid_particle(proton_energy_MeV_u):
    """mass_stopping_power raises TypeError for an unsupported particle argument type."""
    with pytest.raises(TypeError):
        pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, particle="proton")


def test_mass_stopping_power_particle_a_none(proton_energy_MeV_u):
    """mass_stopping_power raises TypeError when a Particle with A=None is passed."""
    particle = pyamtrack.particles.Particle.from_string("H")  # A is None
    assert particle.A is None
    with pytest.raises(Exception, match="Particle.A is None"):
        pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, particle=particle)


def test_mass_stopping_power_invalid_material(proton_energy_MeV_u):
    """mass_stopping_power raises TypeError for an unsupported material argument type."""
    with pytest.raises(TypeError):
        pyamtrack.stopping.mass_stopping_power(proton_energy_MeV_u, material="water")


# ──────────────────────────────────────────────────────────────────────────────
# Tests for stopping_power
# ──────────────────────────────────────────────────────────────────────────────


def test_stopping_power_scalar_positive(proton_energy_MeV_u):
    """stopping_power returns a positive float for a scalar proton energy."""
    result = pyamtrack.stopping.stopping_power(proton_energy_MeV_u)
    assert isinstance(result, float)
    assert result > 0, "Stopping power must be positive"


def test_stopping_power_list_input(proton_energy_MeV_u):
    """stopping_power accepts a list and returns an ndarray of the same length."""
    energies = [proton_energy_MeV_u, 10.0, 1.0]
    result = pyamtrack.stopping.stopping_power(energies)
    assert isinstance(result, np.ndarray)
    assert result.shape == (3,)
    assert np.all(result > 0), "All stopping power values must be positive"


def test_stopping_power_numpy_input():
    """stopping_power accepts a numpy array and returns an ndarray of the same shape."""
    energies = np.logspace(np.log10(0.01), np.log10(500), 20)
    result = pyamtrack.stopping.stopping_power(energies)
    assert isinstance(result, np.ndarray)
    assert result.shape == energies.shape
    assert np.all(result > 0), "All stopping power values must be positive"


def test_stopping_power_particle_object(proton_energy_MeV_u):
    """stopping_power accepts a Particle object constructed via from_number."""
    particle = pyamtrack.particles.Particle.from_number(PROTON_NO)
    result_obj = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle=particle)
    result_int = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle=PROTON_NO)
    assert result_obj == pytest.approx(result_int)


def test_stopping_power_material_object(proton_energy_MeV_u):
    """stopping_power accepts a Material object."""
    material = pyamtrack.materials.water_liquid
    result_obj = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, material=material)
    result_int = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, material=WATER_LIQUID_ID)
    assert result_obj == pytest.approx(result_int)


def test_stopping_power_carbon(proton_energy_MeV_u):
    """stopping_power returns a higher value for carbon than proton at same energy/nucleon."""
    sp_proton = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle=PROTON_NO)
    sp_carbon = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle=CARBON_NO)
    assert sp_carbon > sp_proton, "Carbon should have higher stopping power than proton"


def test_stopping_power_invalid_particle(proton_energy_MeV_u):
    """stopping_power raises TypeError for an unsupported particle argument type."""
    with pytest.raises(TypeError):
        pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle="proton")


def test_stopping_power_particle_a_none(proton_energy_MeV_u):
    """stopping_power raises TypeError when a Particle with A=None is passed."""
    particle = pyamtrack.particles.Particle.from_string("H")  # A is None
    assert particle.A is None
    with pytest.raises(Exception, match="Particle.A is None"):
        pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle=particle)


def test_stopping_power_invalid_material(proton_energy_MeV_u):
    """stopping_power raises TypeError for an unsupported material argument type."""
    with pytest.raises(TypeError):
        pyamtrack.stopping.stopping_power(proton_energy_MeV_u, material="water")


def test_stopping_power_vectorized_particle_list(proton_energy_MeV_u):
    """stopping_power accepts a list of particle IDs and returns an ndarray."""
    result = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, particle=[PROTON_NO, CARBON_NO])
    assert isinstance(result, np.ndarray)
    assert result.shape == (2,)
    assert np.all(result > 0)


def test_stopping_power_vectorized_material_list(proton_energy_MeV_u):
    """stopping_power accepts a list of material IDs and returns an ndarray."""
    result = pyamtrack.stopping.stopping_power(proton_energy_MeV_u, material=[1, 2])
    assert isinstance(result, np.ndarray)
    assert result.shape == (2,)
    assert np.all(result > 0)
