import numpy as np
import pytest

import pyamtrack.materials
import pyamtrack.particles
import pyamtrack.stopping
import pyamtrack.track as track

PROTON = pyamtrack.particles.proton
CARBON = pyamtrack.particles.from_string("12C")
RADII_M = np.logspace(-10, -4, 7)

RADIAL_DOSE_FUNCTIONS = {
    "katz_point": ("r_min_m", "d_min_Gy"),
    "geiss": ("a0_m",),
    "katz_site": ("a0_m", "d_min_Gy"),
    "cucinotta_point": ("r_min_m", "d_min_Gy"),
    "katz_extended_target": ("r_min_m", "a0_m", "d_min_Gy"),
    "cucinotta_extended_target": ("r_min_m", "a0_m", "d_min_Gy"),
}
KATZ_FUNCTIONS = ["katz_point", "katz_site", "katz_extended_target"]


def radial_dose_function(name):
    return getattr(track, name)


def test_rdd_model_enum_is_not_exposed():
    assert not hasattr(track, "RadialDoseDistributionModel")


@pytest.mark.parametrize("name", RADIAL_DOSE_FUNCTIONS)
def test_radial_dose_returns_finite_non_negative_dose_per_radius(name):
    dose = radial_dose_function(name)(RADII_M, 100.0, PROTON)

    assert isinstance(dose, np.ndarray)
    assert dose.shape == RADII_M.shape
    assert np.all(np.isfinite(dose))
    assert np.all(dose >= 0.0)
    assert dose[0] > dose[-1]


@pytest.mark.parametrize("name", RADIAL_DOSE_FUNCTIONS)
def test_radial_dose_supports_keyword_call(name):
    positional = radial_dose_function(name)(RADII_M, 100.0, PROTON, 1)
    keyword = radial_dose_function(name)(radius_m=RADII_M, energy_MeV_u=100.0, particle=PROTON, material=1)

    np.testing.assert_array_equal(positional, keyword)


def test_katz_point_dose_falls_with_inverse_square_radius_near_the_path():
    dose = track.katz_point([1e-9, 1e-8], 100.0, PROTON)

    assert dose[0] / dose[1] == pytest.approx(100.0, rel=1e-3)


def test_scalar_radius_and_configuration_return_float():
    dose = track.katz_point(1e-8, 100.0, PROTON)

    assert isinstance(dose, float)
    assert dose == pytest.approx(track.katz_point([1e-8], 100.0, PROTON)[0])


def test_radius_list_and_numpy_array_are_equivalent():
    np.testing.assert_array_equal(
        track.katz_point(list(RADII_M), 100.0, PROTON),
        track.katz_point(RADII_M, 100.0, PROTON),
    )


def test_scalar_radius_drops_radius_axis_for_vector_configurations():
    dose = track.katz_point(1e-8, [10.0, 100.0], PROTON)

    assert dose.shape == (2,)


def test_elementwise_configurations_add_leading_axis():
    energies = [10.0, 100.0]
    particles = [PROTON, CARBON]
    dose = track.katz_point(RADII_M, energies, particles)

    assert dose.shape == (2, RADII_M.size)
    for i, (energy, particle) in enumerate(zip(energies, particles)):
        np.testing.assert_allclose(dose[i], track.katz_point(RADII_M, energy, particle))


def test_elementwise_configurations_require_matching_lengths():
    with pytest.raises(ValueError):
        track.katz_point(RADII_M, [10.0, 100.0], [PROTON, CARBON, PROTON])


def test_cartesian_product_combines_configuration_arguments():
    energies = np.array([10.0, 100.0, 200.0])
    particles = [PROTON, CARBON]
    dose = track.katz_point(RADII_M, energies, particles, cartesian_product=True)

    assert dose.shape == (3, 2, RADII_M.size)
    for i, energy in enumerate(energies):
        for j, particle in enumerate(particles):
            np.testing.assert_allclose(dose[i, j], track.katz_point(RADII_M, energy, particle))


def test_material_object_matches_material_id():
    np.testing.assert_array_equal(
        track.geiss(RADII_M, 100.0, PROTON, material=pyamtrack.materials.water_liquid),
        track.geiss(RADII_M, 100.0, PROTON, material=1),
    )


@pytest.mark.parametrize("name", RADIAL_DOSE_FUNCTIONS)
def test_explicit_default_parameters_match_omitted_parameters(name):
    function = radial_dose_function(name)
    defaults = {
        "r_min_m": 5e-11 if name.startswith("cucinotta") else 1e-10,
        "a0_m": 1e-8 if name.endswith("extended_target") else 5e-8,
        "d_min_Gy": 1e-10,
    }
    parameters = {key: defaults[key] for key in RADIAL_DOSE_FUNCTIONS[name]}

    np.testing.assert_array_equal(
        function(RADII_M, 100.0, PROTON, rdd_parameters=parameters),
        function(RADII_M, 100.0, PROTON),
    )


def test_rdd_parameters_change_the_result():
    default = track.geiss(RADII_M, 100.0, PROTON)
    wider_core = track.geiss(RADII_M, 100.0, PROTON, rdd_parameters={"a0_m": 1e-7})

    assert not np.allclose(default, wider_core)


@pytest.mark.parametrize("name", RADIAL_DOSE_FUNCTIONS)
def test_rdd_parameters_reject_unknown_keys(name):
    with pytest.raises(ValueError, match="Unknown rdd_parameters key"):
        radial_dose_function(name)(RADII_M, 100.0, PROTON, rdd_parameters={"E_idx": 1.0})


def test_rdd_parameters_reject_keys_of_another_model():
    with pytest.raises(ValueError, match="Unknown rdd_parameters key 'a0_m'"):
        track.katz_point(RADII_M, 100.0, PROTON, rdd_parameters={"a0_m": 1e-8})


@pytest.mark.parametrize(
    "parameters",
    [{"r_min_m": 0.0}, {"r_min_m": -1e-10}, {"r_min_m": np.nan}, {"d_min_Gy": -1.0}, {"a0_m": 0.0}],
)
def test_rdd_parameters_reject_invalid_values(parameters):
    with pytest.raises(ValueError, match="rdd_parameters"):
        track.katz_extended_target(RADII_M, 100.0, PROTON, rdd_parameters=parameters)


def test_rdd_parameters_must_be_a_dict_of_numbers():
    with pytest.raises(TypeError):
        track.katz_point(RADII_M, 100.0, PROTON, rdd_parameters=[1e-10, 1e-10])
    with pytest.raises(TypeError):
        track.katz_point(RADII_M, 100.0, PROTON, rdd_parameters={"r_min_m": "small"})


@pytest.mark.parametrize("name", KATZ_FUNCTIONS)
@pytest.mark.parametrize("model", ["tabata", "geiss", "scholz", pyamtrack.stopping.ElectronRangeModel.TABATA])
def test_katz_models_reject_incompatible_electron_range_models(name, model):
    with pytest.raises(ValueError, match="incompatible with electron_range_model"):
        radial_dose_function(name)(RADII_M, 100.0, PROTON, electron_range_model=model)


@pytest.mark.parametrize("name", KATZ_FUNCTIONS)
@pytest.mark.parametrize("model", ["butts_katz", "waligorski", "edmund", 2])
def test_katz_models_accept_compatible_electron_range_models(name, model):
    dose = radial_dose_function(name)(RADII_M, 100.0, PROTON, electron_range_model=model)

    assert np.all(np.isfinite(dose))
    assert np.any(dose > 0.0)


@pytest.mark.parametrize("name", ["geiss", "cucinotta_point", "cucinotta_extended_target"])
def test_non_katz_models_accept_other_electron_range_models(name):
    dose = radial_dose_function(name)(RADII_M, 100.0, PROTON, electron_range_model="scholz")

    assert np.all(np.isfinite(dose))


@pytest.mark.parametrize("model", ["unknown", 1, 99, True])
def test_invalid_electron_range_models_are_rejected(model):
    with pytest.raises((TypeError, ValueError)):
        track.geiss(RADII_M, 100.0, PROTON, electron_range_model=model)


@pytest.mark.parametrize("radius", [-1e-9, np.nan, np.inf])
def test_invalid_radii_are_rejected(radius):
    with pytest.raises(ValueError, match="radius_m"):
        track.katz_point([1e-9, radius], 100.0, PROTON)


def test_multidimensional_radius_array_is_rejected():
    with pytest.raises(ValueError, match="radius_m"):
        track.katz_point(RADII_M.reshape(1, -1), 100.0, PROTON)


def test_radius_below_r_min_returns_zero_dose():
    assert track.katz_point(1e-12, 100.0, PROTON) == 0.0


@pytest.mark.parametrize("energy", [0.0, -1.0, np.nan, np.inf, 1e6])
def test_invalid_energies_are_rejected(energy):
    with pytest.raises(ValueError, match="energy_MeV_u"):
        track.katz_point(RADII_M, energy, PROTON, source="bethe")


def test_pstar_rejects_energy_outside_table_range():
    with pytest.raises(ValueError, match="outside the PSTAR range"):
        track.katz_point(RADII_M, 20000.0, PROTON, source="pstar")


def test_default_source_requires_bethe_fallback_outside_pstar_range():
    with pytest.raises(ValueError, match="Bethe fallback"):
        track.katz_point(RADII_M, 20000.0, PROTON)

    np.testing.assert_allclose(
        track.katz_point(RADII_M, 20000.0, PROTON, allow_bethe_fallback=True),
        track.katz_point(RADII_M, 20000.0, PROTON, source="bethe"),
    )


def test_icru_rejects_unsupported_material():
    with pytest.raises(ValueError, match="ICRU"):
        track.katz_point(RADII_M, 100.0, PROTON, material=24, source="icru")


@pytest.mark.parametrize("particle", [1001, pyamtrack.particles.from_string("electron"), True])
def test_non_ion_particles_are_rejected(particle):
    with pytest.raises(TypeError):
        track.katz_point(RADII_M, 100.0, particle)


@pytest.mark.parametrize("material", [0, 999, True])
def test_invalid_materials_are_rejected(material):
    with pytest.raises((TypeError, ValueError)):
        track.katz_point(RADII_M, 100.0, PROTON, material=material)


def test_track_width_uses_electron_range_models():
    for model in ["tabata", "waligorski", pyamtrack.stopping.ElectronRangeModel.GEISS]:
        assert track.track_width(100.0, 1, model) == pytest.approx(
            pyamtrack.stopping.electron_range(100.0, 1, model)
        )


def test_track_width_defaults_to_tabata():
    assert track.track_width(100.0) == pytest.approx(pyamtrack.stopping.electron_range(100.0, 1, "tabata"))


def test_track_width_rejects_invalid_models():
    with pytest.raises(ValueError):
        track.track_width(100.0, 1, "unknown")
