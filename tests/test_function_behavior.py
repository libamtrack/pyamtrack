import numpy as np
import pytest

import pyamtrack.particles
from pyamtrack.converters import beta_from_energy, energy_from_beta
from pyamtrack.stopping import electron_range


PROTON = pyamtrack.particles.proton


test_cases = [
    (beta_from_energy, {"particle": PROTON}, 0, 1000),
    (energy_from_beta, {"particle": PROTON}, 0, 1),
    (electron_range, {}, 0, 1000),
]


@pytest.mark.parametrize("func, kwargs, min_val, max_val", test_cases)
def test_function_behavior(func, kwargs, min_val, max_val):
    assert func(0, **kwargs) == 0

    input_list = [min_val, (min_val + max_val) / 2, max_val]
    input_array = np.array(input_list)

    output_list = func(input_list, **kwargs)
    output_array = func(input_array, **kwargs)

    assert np.allclose(output_list, output_array)
    assert isinstance(output_array, np.ndarray)

    negative_input = -1 * (min_val + 1)
    result = func(negative_input, **kwargs)

    assert np.isnan(result) or np.isinf(result)

    with pytest.raises(TypeError):
        func("string", **kwargs)

    with pytest.raises(TypeError):
        func(None, **kwargs)

    with pytest.raises(TypeError):
        func([1, "string", 3], **kwargs)

    inputs = np.linspace(min_val, max_val, 100)
    outputs = func(inputs, **kwargs)

    assert np.all(np.diff(outputs) >= 0)