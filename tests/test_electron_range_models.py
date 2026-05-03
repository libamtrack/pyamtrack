import numpy as np
import pytest

import pyamtrack.electron_range as electron_range
import pyamtrack.materials


@pytest.fixture
def electron_energy_MeV():
    """Fixture providing the electron energy in MeV for tests."""
    return 100.0


@pytest.fixture
def models():
    """Fixture providing all available models."""
    return electron_range.get_models()


def test_available_models(models):
    """Test that all expected models are available."""
    expected_models = {
        "butts_katz",
        "waligorski",
        "geiss",
        "scholz",
        "edmund",
        "tabata",
        "scholz_new",
    }
    assert set(models) == expected_models


def test_model_id_mapping():
    """Test that model names map to correct IDs."""
    assert electron_range.model("butts_katz") == 2
    assert electron_range.model("tabata") == 7
    assert electron_range.model("scholz_new") == 8


def test_invalid_model(electron_energy_MeV):
    """Test handling of invalid model names."""
    with pytest.raises(ValueError, match="Unknown model name: invalid_model"):
        electron_range.max_range(electron_energy_MeV, model="invalid_model")
    with pytest.raises(TypeError):
        electron_range.max_range(electron_energy_MeV, model=None)


@pytest.mark.parametrize(
    "model_name",
    ["butts_katz", "waligorski", "geiss", "scholz", "edmund", "tabata", "scholz_new"],
)
def test_model_output_validity(electron_energy_MeV, model_name):
    """Test that each model produces physically meaningful results."""
    range_m = electron_range.max_range(electron_energy_MeV, model=model_name)
    assert range_m > 0, f"{model_name} model returned negative range"
    assert range_m < 1000, f"{model_name} model returned unreasonably large range"


def test_model_consistency(electron_energy_MeV):
    """Test that models can be specified by both name and ID."""
    range_by_name = electron_range.max_range(electron_energy_MeV, model="tabata")
    range_by_id = electron_range.max_range(electron_energy_MeV, model=7)
    assert range_by_name == range_by_id


def test_model_relative_ranges(electron_energy_MeV, models):
    """Test relative behavior of different models.

    While models may give different results, they should all be within
    reasonable physical bounds of each other for the same input."""
    ranges = [electron_range.max_range(electron_energy_MeV, model=m) for m in models]
    max_range = max(ranges)
    min_range = min(ranges)

    # Check that ranges from different models don't differ by more than a factor of 25
    # Models can legitimately differ by larger factors due to different theoretical approaches
    assert max_range / min_range < 25, "Models differ too drastically"


def test_energy_scaling():
    """Test that range increases with energy across all models."""
    energies = np.array([10.0, 100.0])
    for model in electron_range.get_models():
        ranges = electron_range.max_range(energies, model=model)
        assert ranges[1] > ranges[0], f"{model} model doesn't show expected energy scaling"


def test_material_independence(electron_energy_MeV, models):
    """Test that models work with different materials."""
    materials = [1, pyamtrack.materials.water_liquid, pyamtrack.materials.air]

    for model in models:
        ranges = [electron_range.max_range(electron_energy_MeV, material=m, model=model) for m in materials]
        assert all(r > 0 for r in ranges), f"{model} failed with some material"
