from typing import Tuple, Union

import numpy as np
import pytest

from pyamtrack.stopping import electron_range

Shape = Union[int, tuple[int, ...]]


def check_correct_shape(output: np.ndarray, size1: Shape, size2: Shape, size3: Shape) -> bool:
    """
    Check whether a NumPy array has the expected shape based on three size specifications.

    If one of the sizes is zero, it means that an empty array or empty list was given,
    and the expected output should also be an empty array. In this case, the function
    returns True, meaning no further checks are needed.

    Otherwise, the expected output shape is a concatenation of the given shapes.
    """

    # Ensure each size is a tuple, then concatenate them into a single tuple
    sizes = []
    for s in (size1, size2, size3):
        if isinstance(s, int):
            sizes.append((s,))
        else:
            sizes.append(s)

    # Concatenate all tuples, into one
    output_shape = sum(sizes, ())
    if min(output_shape) <= 0:
        assert output.shape == (0,)
        return True
    else:
        assert output.shape == output_shape
        return False


def draw_random_coords(size1: Shape, size2: Shape, size3: Shape) -> list[tuple[int, ...]]:
    """
    Generate random coordinates based on three size specifications.
    """
    rand_coords = []

    for size in (size1, size2, size3):
        if isinstance(size, int):
            coord = (np.random.randint(size),)
        else:
            coord = tuple(np.random.randint(s) for s in size)
        rand_coords.append(coord)

    return rand_coords


def compare_coords(output: np.ndarray, inputs: tuple, coords: list[tuple[int, ...]]):
    flattened_coords = tuple([s for shape in coords for s in shape])

    args = []
    for inp, coord in zip(inputs, coords):
        if isinstance(inp, list):
            args.append(inp[coord[0]])
        else:
            args.append(inp[coord])

    assert np.allclose(
        output[flattened_coords].item(),
        electron_range(float(args[0]), int(args[1]), int(args[2])),
    )


@pytest.mark.parametrize(
    "size1, size2, size3",
    [
        (1, 1, 1),
        (1, 1, 5),
        (1, (7, 10), 1),
        ((10, 2), (1, 5), 1),
        (2, 5, 5),
        (2, 5, 1),
        ((10, 5), 10, 10),
        ((100, 10), (3, 20), (50, 2)),
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
        (10, (10, 5), 1, 0),
        (10, 20, 30, 1),
        (1, (2, 5), 5, 2),
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
        (1, (10, 5), 5),
        ((10, 4), 1, 30),
        (1, (2, 3), 1),
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

    for i, size in zip(range(3), [size1, size2, size3]):
        if size == 1:
            if i >= 1:
                args[i] = int(args[i][0].item())
            else:
                args[i] = float(args[i][0].item())

    output = electron_range(
        *args,
        cartesian_product=True,
    )

    expected_shape = ()

    for s in (size1, size2, size3):
        if isinstance(s, tuple):
            expected_shape += s
        elif s > 1:
            expected_shape += (s,)

    assert output.shape == expected_shape
