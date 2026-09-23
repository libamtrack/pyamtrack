import numpy as np
import pytest

import pyamtrack.materials
import pyamtrack.particles
import pyamtrack.stopping

PROTON_NO = 1001
CARBON_NO = 6012
PROTON = pyamtrack.particles.proton
CARBON = pyamtrack.particles.from_string("12C")
HELIUM = pyamtrack.particles.He


def test_stopping_power_functions_support_scalar_and_energy_sequences():
    energies = np.array([1.0, 10.0, 100.0])

    for function in (pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power):
        scalar = function(100.0, particle=PROTON)
        values = function(energies, particle=PROTON)

        assert isinstance(scalar, float)
        assert isinstance(values, np.ndarray)
        assert values.shape == energies.shape
        assert np.all(np.isfinite(values))
        assert np.all(values > 0.0)


@pytest.mark.parametrize("function", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_stopping_power_rejects_integer_particle_arguments(function):
    with pytest.raises(TypeError):
        function(100.0, particle=PROTON_NO)


def test_mass_and_linear_stopping_power_have_expected_units():
    material = pyamtrack.materials.water_liquid
    mass_value = pyamtrack.stopping.mass_stopping_power(100.0, particle=PROTON, material=material, source="pstar")
    linear_value = pyamtrack.stopping.stopping_power(100.0, particle=PROTON, material=material, source="pstar")

    expected_linear_value = mass_value * material.density_g_cm3 / 10.0
    assert linear_value == pytest.approx(expected_linear_value)


def test_default_source_supports_material_dependent_defaults():
    result = pyamtrack.stopping.mass_stopping_power(
        100.0,
        particle=PROTON,
        material=[1, 24],
        source="default",
        allow_bethe_fallback=True,
    )
    expected = np.array(
        [
            pyamtrack.stopping.mass_stopping_power(100.0, particle=PROTON, material=1, source="pstar"),
            pyamtrack.stopping.mass_stopping_power(
                100.0,
                particle=PROTON,
                material=24,
                source="bethe",
                allow_bethe_fallback=True,
            ),
        ]
    )

    assert result.shape == (2,)
    np.testing.assert_allclose(result, expected)


def test_stopping_power_functions_support_cartesian_products():
    energies = np.array([10.0, 100.0])
    particles = [PROTON, CARBON]

    for function in (pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power):
        result = function(
            energies,
            particle=particles,
            source="bethe",
            allow_bethe_fallback=True,
            cartesian_product=True,
        )
        expected = np.array(
            [
                [
                    function(energy, particle=particle, source="bethe", allow_bethe_fallback=True)
                    for particle in particles
                ]
                for energy in energies
            ]
        )

        assert result.shape == (2, 2)
        np.testing.assert_allclose(result, expected)


def test_full_output_reports_the_resolved_scalar_source():
    for function in (pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power):
        result = function(100.0, particle=PROTON, source="pstar", full_output=True)

        assert result._fields == ("value", "source_id")
        values, source_id = result
        assert isinstance(values, float)
        assert source_id == 2
        assert result.value == values
        assert result.source_id == source_id


def test_full_output_matches_vector_result_shape():
    energies = np.array([1.0, 10.0, 100.0])

    for function in (pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power):
        values, source_ids = function(
            energies,
            particle=PROTON,
            source="bethe",
            allow_bethe_fallback=True,
            full_output=True,
        )

        assert values.shape == energies.shape
        assert source_ids.shape == energies.shape
        assert np.issubdtype(source_ids.dtype, np.integer)
        np.testing.assert_array_equal(source_ids, 1)


def test_full_output_reports_material_dependent_default_sources():
    values, source_ids = pyamtrack.stopping.mass_stopping_power(
        100.0,
        particle=PROTON,
        material=[1, 24],
        source="default",
        allow_bethe_fallback=True,
        full_output=True,
    )

    assert values.shape == (2,)
    np.testing.assert_array_equal(source_ids, np.array([2, 1]))


def test_full_output_matches_cartesian_result_shape():
    energies = np.array([10.0, 100.0])
    materials = [1, 24]

    values, source_ids = pyamtrack.stopping.stopping_power(
        energies,
        particle=PROTON,
        material=materials,
        source="default",
        allow_bethe_fallback=True,
        cartesian_product=True,
        full_output=True,
    )

    assert values.shape == (2, 2)
    assert source_ids.shape == values.shape
    np.testing.assert_array_equal(source_ids, np.array([[2, 1], [2, 1]]))


@pytest.mark.parametrize("function", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
@pytest.mark.parametrize("energy", [0.000999, 10000.001])
def test_stopping_power_rejects_energy_outside_pstar_range(function, energy):
    with pytest.raises(ValueError, match="outside the PSTAR range"):
        function(energy, particle=PROTON, source="pstar")


@pytest.mark.parametrize("function", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_default_source_falls_back_to_bethe_outside_pstar_range(function):
    energies = [10.0, 100.0, 1_000_000_000.0]
    values, source_ids = function(
        energies,
        particle=PROTON,
        material=pyamtrack.materials.water_liquid,
        source="default",
        allow_bethe_fallback=True,
        full_output=True,
    )
    expected = np.array(
        [
            function(10.0, particle=PROTON, material=1, source="pstar"),
            function(100.0, particle=PROTON, material=1, source="pstar"),
            function(
                1_000_000_000.0,
                particle=PROTON,
                material=1,
                source="bethe",
                allow_bethe_fallback=True,
            ),
        ]
    )

    np.testing.assert_array_equal(source_ids, np.array([2, 2, 1]))
    np.testing.assert_allclose(values, expected)


@pytest.mark.parametrize("function", [pyamtrack.stopping.mass_stopping_power, pyamtrack.stopping.stopping_power])
def test_icru_rejects_helium_energy_above_table_range(function):
    with pytest.raises(ValueError, match="outside the ICRU range"):
        function(250.001, particle=HELIUM, source="icru")
