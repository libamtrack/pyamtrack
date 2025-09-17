from random import uniform

import numpy as np
import pytest

from pyamtrack.stopping import electron_range


@pytest.mark.parametrize(
    "param_1_size, param_2_size, param_3_size",
    [
        (1, 1, 1),
        (1, 1, 5),
        (1, 7, 1),
        (10, 1, 1),
        (2, 5, 5),
        (2, 5, 1),
        (10, 10, 10),
        (100, 20, 50),
        (10, 0, 2),
        (0, 10, 5),
    ],
)
def test_cartesian_product(param_1_size, param_2_size, param_3_size):
    """Simple tests for cartesian product"""

    args = (
        np.random.uniform(900, 1100, param_1_size),
        np.random.randint(2, 8, param_2_size),
        np.random.randint(2, 8, param_3_size),
    )

    new_version_output = electron_range(
        *args,
        cartesian_product=True,
        new_version=True,
    )

    old_version_output = electron_range(
        *args,
        cartesian_product=True,
        new_version=False,
    )

    assert np.allclose(new_version_output, old_version_output)

    output_shape = (param_1_size, param_2_size, param_3_size)
    if min(output_shape) <= 0:
        assert new_version_output.shape == (0,)
        return
    else:
        assert new_version_output.shape == output_shape

    rand_cords = (
        np.random.randint(0, param_1_size),
        np.random.randint(0, param_2_size),
        np.random.randint(0, param_3_size),
    )

    assert np.allclose(
        new_version_output[*rand_cords],
        electron_range(args[0][rand_cords[0]], args[1][rand_cords[1]].item(), args[2][rand_cords[2]].item()),
    )


@pytest.mark.parametrize(
    "param_1_size, param_2_size, param_3_size, arg_to_list",
    [
        (10, 10, 1, 0),
        (10, 20, 30, 1),
        (1, 2, 5, 2),
        (1, 2, 1, 0),
    ],
)
def test_with_list_input(param_1_size, param_2_size, param_3_size, arg_to_list):
    """Tests with one of the arguments being a list"""
    args = [
        np.random.uniform(900, 1100, param_1_size),
        np.random.randint(2, 8, param_2_size),
        np.random.randint(2, 8, param_3_size),
    ]

    args[arg_to_list] = args[arg_to_list].tolist()

    new_version_output = electron_range(
        *args,
        cartesian_product=True,
        new_version=True,
    )

    old_version_output = electron_range(
        *args,
        cartesian_product=True,
        new_version=False,
    )

    assert np.allclose(new_version_output, old_version_output)


@pytest.mark.parametrize(
    "param_1_size, param_2_size, param_3_size, scalar_arg",
    [
        (10, 10, 1, 0),
        (10, 20, 30, 1),
        (1, 2, 5, 2),
        (1, 2, 1, 0),
    ],
)
def test_with_scalar_input(param_1_size, param_2_size, param_3_size, scalar_arg):
    """Tests with one of the arguments being a scalar"""

    args = [
        np.random.uniform(900, 1100, param_1_size),
        np.random.randint(2, 8, param_2_size),
        np.random.randint(2, 8, param_3_size),
    ]

    args[scalar_arg] = args[scalar_arg][0].item()

    new_version_output = electron_range(
        *args,
        cartesian_product=True,
        new_version=True,
    )

    old_version_output = electron_range(
        *args,
        cartesian_product=True,
        new_version=False,
    )

    assert np.allclose(new_version_output, old_version_output)
