import numpy as np
import pytest

import pyamtrack
from pyamtrack import proton_models


DEPTH_CM = 10.0
ENERGY_MEV = 150.0
FLUENCE_CM2 = 1e8
ENERGY_SPREAD = 0.01


def test_dose_bortfeld_returns_positive_scalar_dose():
    dose = proton_models.dose_bortfeld(DEPTH_CM, FLUENCE_CM2, ENERGY_MEV)

    assert isinstance(dose, float)
    assert np.isfinite(dose)
    assert dose > 0.0


def test_dose_bortfeld_scales_with_fluence():
    dose = proton_models.dose_bortfeld(DEPTH_CM, FLUENCE_CM2, ENERGY_MEV)
    doubled_dose = proton_models.dose_bortfeld(DEPTH_CM, 2 * FLUENCE_CM2, ENERGY_MEV)

    assert doubled_dose == pytest.approx(2 * dose, rel=1e-12)


def test_dose_bortfeld_accepts_material_objects_and_vector_inputs():
    depths = np.array([2.0, 8.0, 12.0])
    material = pyamtrack.materials.water_liquid

    result = proton_models.dose_bortfeld(
        depths,
        FLUENCE_CM2,
        ENERGY_MEV,
        material=material,
    )
    expected = np.array(
        [
            proton_models.dose_bortfeld(depth, FLUENCE_CM2, ENERGY_MEV, material=1)
            for depth in depths
        ]
    )

    assert isinstance(result, np.ndarray)
    assert result.shape == depths.shape
    np.testing.assert_allclose(result, expected)


def test_dose_bortfeld_cartesian_product_matches_scalar_calls():
    depths = np.array([2.0, 8.0])
    energies = np.array([100.0, 150.0])

    result = proton_models.dose_bortfeld(
        depths,
        FLUENCE_CM2,
        energies,
        cartesian_product=True,
    )
    expected = np.array(
        [
            [
                proton_models.dose_bortfeld(depth, FLUENCE_CM2, energy)
                for energy in energies
            ]
            for depth in depths
        ]
    )

    assert result.shape == (len(depths), len(energies))
    np.testing.assert_allclose(result, expected)


@pytest.mark.parametrize(
    "kwargs",
    [
        {"depth_cm": -1.0},
        {"energy_MeV": 0.09},
        {"energy_MeV": 10000.1},
        {"energy_spread_fraction": 0.0},
        {"energy_spread_fraction": 1.0},
        {"eps": -0.1},
        {"eps": 1.0},
    ],
)
def test_dose_bortfeld_rejects_invalid_scalar_arguments(kwargs):
    arguments = {
        "depth_cm": DEPTH_CM,
        "fluence_cm2": FLUENCE_CM2,
        "energy_MeV": ENERGY_MEV,
        "energy_spread_fraction": ENERGY_SPREAD,
        "material": 1,
        "eps": 0.03,
    }
    arguments.update(kwargs)

    with pytest.raises(ValueError):
        proton_models.dose_bortfeld(**arguments)


def test_dose_bortfeld_rejects_invalid_material():
    with pytest.raises(ValueError):
        proton_models.dose_bortfeld(DEPTH_CM, FLUENCE_CM2, ENERGY_MEV, material=0)

    with pytest.raises(TypeError):
        proton_models.dose_bortfeld(DEPTH_CM, FLUENCE_CM2, ENERGY_MEV, material=True)

    with pytest.raises(TypeError):
        proton_models.dose_bortfeld(DEPTH_CM, FLUENCE_CM2, ENERGY_MEV, material="water_liquid")


def test_let_wilkens_supports_string_and_enum_averaging():
    dose_from_string = proton_models.let_wilkens(
        DEPTH_CM,
        ENERGY_MEV,
        averaging="dose",
    )
    dose_from_uppercase_string = proton_models.let_wilkens(
        DEPTH_CM,
        ENERGY_MEV,
        averaging="DOSE",
    )
    dose_from_enum = proton_models.let_wilkens(
        DEPTH_CM,
        ENERGY_MEV,
        averaging=proton_models.Averaging.DOSE,
    )
    track_from_string = proton_models.let_wilkens(
        DEPTH_CM,
        ENERGY_MEV,
        averaging="track",
    )
    track_from_uppercase_string = proton_models.let_wilkens(
        DEPTH_CM,
        ENERGY_MEV,
        averaging="TRACK",
    )
    track_from_enum = proton_models.let_wilkens(
        DEPTH_CM,
        ENERGY_MEV,
        averaging=proton_models.Averaging.TRACK,
    )

    assert dose_from_string == pytest.approx(dose_from_enum)
    assert dose_from_uppercase_string == pytest.approx(dose_from_string)
    assert track_from_string == pytest.approx(track_from_enum)
    assert track_from_uppercase_string == pytest.approx(track_from_string)
    assert dose_from_string >= track_from_string


def test_let_wilkens_returns_positive_vectorized_let():
    depths = np.array([2.0, 8.0, 12.0])

    dose_averaged = proton_models.let_wilkens(
        depths,
        ENERGY_MEV,
        energy_spread_fraction=ENERGY_SPREAD,
        material=pyamtrack.materials.water_liquid,
        averaging="dose",
    )
    track_averaged = proton_models.let_wilkens(
        depths,
        ENERGY_MEV,
        energy_spread_fraction=ENERGY_SPREAD,
        material=1,
        averaging="track",
    )

    assert isinstance(dose_averaged, np.ndarray)
    assert dose_averaged.shape == depths.shape
    assert np.all(np.isfinite(dose_averaged))
    assert np.all(dose_averaged > 0.0)
    assert np.all(np.isfinite(track_averaged))
    assert np.all(track_averaged > 0.0)
    np.testing.assert_array_less(track_averaged, dose_averaged + 1e-12)


def test_let_wilkens_cartesian_product_matches_scalar_calls():
    depths = [2.0, 8.0]
    energies = [100.0, 150.0]

    result = proton_models.let_wilkens(
        depths,
        energies,
        averaging=proton_models.Averaging.TRACK,
        cartesian_product=True,
    )
    expected = np.array(
        [
            [
                proton_models.let_wilkens(
                    depth,
                    energy,
                    averaging="track",
                )
                for energy in energies
            ]
            for depth in depths
        ]
    )

    assert result.shape == (len(depths), len(energies))
    np.testing.assert_allclose(result, expected)


@pytest.mark.parametrize(
    "kwargs",
    [
        {"depth_cm": -1.0},
        {"energy_MeV": 0.09},
        {"energy_MeV": 10000.1},
        {"energy_spread_fraction": 0.0},
        {"energy_spread_fraction": 1.0},
    ],
)
def test_let_wilkens_rejects_invalid_scalar_arguments(kwargs):
    arguments = {
        "depth_cm": DEPTH_CM,
        "energy_MeV": ENERGY_MEV,
        "energy_spread_fraction": ENERGY_SPREAD,
        "material": 1,
        "averaging": "dose",
    }
    arguments.update(kwargs)

    with pytest.raises(ValueError):
        proton_models.let_wilkens(**arguments)


def test_let_wilkens_rejects_invalid_averaging_and_material():
    with pytest.raises(ValueError):
        proton_models.let_wilkens(DEPTH_CM, ENERGY_MEV, averaging="fluence")

    with pytest.raises(TypeError):
        proton_models.let_wilkens(DEPTH_CM, ENERGY_MEV, averaging=1)

    with pytest.raises(ValueError):
        proton_models.let_wilkens(DEPTH_CM, ENERGY_MEV, material=0)

    with pytest.raises(TypeError):
        proton_models.let_wilkens(DEPTH_CM, ENERGY_MEV, material=True)


def test_proton_models_reject_incompatible_vector_lengths():
    with pytest.raises(ValueError, match="Incompatible lists/arrays size"):
        proton_models.dose_bortfeld(
            [2.0, 8.0],
            [FLUENCE_CM2],
            ENERGY_MEV,
        )

    with pytest.raises(ValueError, match="Incompatible lists/arrays size"):
        proton_models.let_wilkens(
            [2.0, 8.0],
            [ENERGY_MEV],
        )
