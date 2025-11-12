import numpy as np
import pytest

import pyamtrack.materials
import pyamtrack.stopping as stopping


@pytest.fixture
def models():
    """Fixture providing all available models."""
    return stopping.get_models()


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
    assert stopping.model("butts_katz") == 2
    assert stopping.model("tabata") == 7
    assert stopping.model("scholz_new") == 8


def test_invalid_model(electron_energy_MeV_low):
    """Test handling of invalid model names."""
    with pytest.raises(ValueError, match="Unknown model name: invalid_model"):
        stopping.electron_range(electron_energy_MeV_low, model="invalid_model")
    with pytest.raises(TypeError):
        stopping.electron_range(electron_energy_MeV_low, model=None)


@pytest.mark.parametrize(
    "model_name",
    ["butts_katz", "waligorski", "geiss", "scholz", "edmund", "tabata", "scholz_new"],
)
def test_model_output_validity(electron_energy_MeV_low, model_name):
    """Test that each model produces physically meaningful results."""
    range_m = stopping.electron_range(electron_energy_MeV_low, model=model_name)
    assert range_m > 0, f"{model_name} model returned negative range"
    assert range_m < 1000, f"{model_name} model returned unreasonably large range"


def test_model_consistency(electron_energy_MeV_low):
    """Test that models can be specified by both name and ID."""
    range_by_name = stopping.electron_range(electron_energy_MeV_low, model="tabata")
    range_by_id = stopping.electron_range(electron_energy_MeV_low, model=7)
    assert range_by_name == range_by_id


def test_model_relative_ranges(electron_energy_MeV_low, models):
    """Test relative behavior of different models.

    While models may give different results, they should all be within
    reasonable physical bounds of each other for the same input."""
    ranges = [stopping.electron_range(electron_energy_MeV_low, model=m) for m in models]
    max_range = max(ranges)
    min_range = min(ranges)

    # Check that ranges from different models don't differ by more than a factor of 25
    # Models can legitimately differ by larger factors due to different theoretical approaches
    assert max_range / min_range < 25, "Models differ too drastically"


def test_energy_scaling():
    """Test that range increases with energy across all models."""
    energies = np.array([10.0, 100.0])
    for model in stopping.get_models():
        ranges = stopping.electron_range(energies, model=model)
        assert ranges[1] > ranges[0], f"{model} model doesn't show expected energy scaling"


def test_material_independence(electron_energy_MeV_low, models):
    """Test that models work with different materials."""
    materials = [1, pyamtrack.materials.water_liquid, pyamtrack.materials.air]

    for model in models:
        ranges = [stopping.electron_range(electron_energy_MeV_low, material=m, model=model) for m in materials]
        assert all(r > 0 for r in ranges), f"{model} failed with some material"
