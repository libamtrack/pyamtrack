import pyamtrack.stopping


def test_electron_range():
    """Test the electron_range function for various inputs."""
    energy = 1000.0  # MeV
    range_m = pyamtrack.stopping.electron_range(energy)
    assert range_m > 0.01, "Expected positive range for positive energy."
