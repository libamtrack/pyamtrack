import numpy as np
import pyamtrack.particles

from pyamtrack.converters import beta_from_energy, energy_from_beta


PROTON = pyamtrack.particles.proton
CARBON = pyamtrack.particles.from_string("12C")


def test_beta_from_energy_accepts_energy_array_with_scalar_particle():
    energies = np.array([10.0, 100.0, 200.0])

    result = beta_from_energy(energies, particle=PROTON)

    expected = np.array([
        beta_from_energy(energy, particle=PROTON)
        for energy in energies
    ])

    assert result.shape == energies.shape
    np.testing.assert_allclose(result, expected)


def test_energy_from_beta_accepts_beta_array_with_scalar_particle():
    betas = np.array([0.1, 0.5, 0.8])

    result = energy_from_beta(betas, particle=PROTON)

    expected = np.array([
        energy_from_beta(beta, particle=PROTON)
        for beta in betas
    ])

    assert result.shape == betas.shape
    np.testing.assert_allclose(result, expected)


def test_beta_from_energy_cartesian_product():
    energies = np.array([10.0, 100.0])
    particles = [PROTON, CARBON]

    result = beta_from_energy(
        energies,
        particle=particles,
        cartesian_product=True,
    )

    expected = np.array([
        [
            beta_from_energy(energy, particle=particle)
            for particle in particles
        ]
        for energy in energies
    ])

    assert result.shape == expected.shape
    np.testing.assert_allclose(result, expected)