import numpy as np
import pytest

from pyamtrack.stopping import electron_range


def check_correct_shape(output: np.ndarray, size1: int, size2: int, size3: int) -> bool:
    output_shape = (size1, size2, size3)
    if min(output_shape) <= 0:
        assert output.shape == (0,)
        return True
    else:
        assert output.shape == output_shape
        return False


def draw_random_coords(size1: int, size2: int, size3: int) -> tuple[int, int, int]:
    rand_coords = (
        np.random.randint(size1),
        np.random.randint(size2),
        np.random.randint(size3),
    )
    return rand_coords


def compare_coords(output: np.ndarray, args: tuple, coords: tuple[int, int, int]):
    assert np.allclose(
        output[*coords].item(),
        electron_range(float(args[0][coords[0]]), int(args[1][coords[1]]), int(args[2][coords[2]])),
    )


@pytest.mark.parametrize(
    "size1, size2, size3",
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
def test_cartesian_product(size1, size2, size3) -> None:
    """Simple tests for cartesian product"""

    args = (
        np.random.uniform(900, 1100, size1),
        np.random.randint(2, 8, size2),
        np.random.randint(2, 8, size3),
    )

    output = electron_range(
        *args,
        cartesian_product=True,
    )

    if check_correct_shape(output, size1, size2, size3):
        return

    for _ in range(100):
        rand_coords = draw_random_coords(size1, size2, size3)
        compare_coords(output, args, rand_coords)


@pytest.mark.parametrize(
    "size1, size2, size3, arg_to_list",
    [
        (10, 10, 1, 0),
        (10, 20, 30, 1),
        (1, 2, 5, 2),
        (1, 2, 1, 0),
    ],
)
def test_with_list_input(size1, size2, size3, arg_to_list):
    """Tests with one of the arguments being a list"""
    args = [
        np.random.uniform(900, 1100, size1),
        np.random.randint(2, 8, size2),
        np.random.randint(2, 8, size3),
    ]

    args[arg_to_list] = args[arg_to_list].tolist()

    output = electron_range(
        *args,
        cartesian_product=True,
    )

    if check_correct_shape(output, size1, size2, size3):
        return

    for _ in range(100):
        rand_coords = draw_random_coords(size1, size2, size3)
        compare_coords(output, args, rand_coords)


@pytest.mark.parametrize(
    "size1, size2, size3",
    [
        (1, 10, 5),
        (10, 1, 30),
        (1, 2, 1),
        (1, 2, 5),
    ],
)
def test_with_scalar_input(size1, size2, size3):
    """Tests with one of the arguments being a scalar"""

    args = [
        np.random.uniform(900, 1100, size1),
        np.random.randint(2, 8, size2),
        np.random.randint(2, 8, size3),
    ]

    for i in range(3):
        if len(args) <= 1:
            args[i] = args[i][0].item()

    output = electron_range(
        *args,
        cartesian_product=True,
    )

    if check_correct_shape(output, size1, size2, size3):
        return
