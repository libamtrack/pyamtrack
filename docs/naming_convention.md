# Naming conventions for Python-bound arguments

This document describes the argument names and calling conventions of the
Python API implemented with [nanobind](https://nanobind.readthedocs.io/).
It intentionally covers only the bindings in `src/**/*_bindings.cpp`; C++
helpers that are not exposed through nanobind are out of scope.

The `nb::arg("...")` declaration is the source of truth for a keyword name in
Python. A name used only in a C++ signature or docstring is a semantic label,
not necessarily a supported keyword. In particular, the current constructors
and several older `m.def(...)` calls do not declare `nb::arg(...)`; keep their
documented arguments positional unless explicit argument names are added to
the binding.

## General rules

- Use `snake_case` for Python-facing names.
- Use a descriptive physical quantity rather than a one-letter implementation
  variable. For example, expose `energy_MeV` instead of the internal `E_MeV`.
- Put the unit in the name when the unit is fixed: `depth_cm`,
  `energy_MeV`, `energy_MeV_u`, and `fluence_cm2`.
- Use `_fraction` for a dimensionless fraction. It means `0.01` for one
  percent, not `1`.
- Preserve established scientific notation where it is part of the domain:
  `Z` is atomic number, `A` is mass number, and `pdg_code` is a PDG code.
- Use `material` for either a material ID or a `pyamtrack.materials.Material`
  object. Do not introduce separate names such as `material_id` for APIs that
  accept both forms.
- Use `model` for a selectable calculation model and `averaging` for the LET
  averaging convention.
- Boolean switches use a positive `snake_case` name, such as
  `cartesian_product`, and default to `False` unless there is a strong reason
  otherwise.

The public name may deliberately differ from the C++ implementation name.
For example, `dose_bortfeld` receives `depth_cm`, `energy_MeV`, and
`energy_spread_fraction` in Python, while its C++ implementation uses
`z_cm`, `E_MeV`, and `sigma_E_fraction`.

## Scalar, list, and NumPy arguments

The numerical wrappers generally accept:

- a Python `int` or `float` for one value;
- a Python `list` for a sequence of values; or
- a NumPy array where the function documentation says arrays are supported.

For the converter functions, a list produces a list and a NumPy array
produces a NumPy array with the corresponding shape. The multi-argument
calculation functions (`electron_range`, `dose_bortfeld`, and `let_wilkens`)
return a scalar when every argument is scalar. If at least one argument is a
sequence, they return a one-dimensional NumPy array, including when the
sequence is a Python list. Scalar arguments are repeated and sequence
arguments are evaluated elementwise; sequence lengths must match.

Set `cartesian_product=True` on the three multi-argument calculation
functions to evaluate every combination of sequence values. The result is a
NumPy array whose dimensions follow the iterable arguments. The
`averaging` argument of `let_wilkens` is a single convention selector, not an
independent product dimension.

NumPy inputs must be compatible with the wrapper:

- elementwise multi-argument calls require one-dimensional arrays;
- elementwise calls accept non-contiguous arrays, such as a strided slice;
- cartesian-product calls require NumPy arrays to be C-contiguous; and
- numeric arrays must be castable to the required numeric type.

When adding a new binding, document accepted scalar/container types, whether
scalars are broadcast, the output type, and any shape restrictions in the
nanobind docstring.

## Current Python API

The following is the current inventory of functions and constructor
arguments exposed through nanobind.

### `pyamtrack.converters`

- `beta_from_energy(energy_MeV_u)`
- `energy_from_beta(beta)`

### `pyamtrack.materials`

- `Material(id)` — construct from a material ID.
- `Material(name)` — construct from a material name.
- `get_ids()` — return the valid material IDs.
- `get_long_names()`
- `get_names()`

### `pyamtrack.particles`

- `Particle(pdg_code)` — construct a non-ion particle from a PDG code.
- `get_names()`
- `get_acronyms()`
- `from_string(name)`
- `from_pdg(pdg_code)`
- `from_ZA(Z, A)`
- `particles.ions.Ion(pdg_code)` — construct an ion from a nuclear PDG
  code.

### `pyamtrack.stopping`

- `get_models()`
- `model(name)`
- `electron_range(energy_MeV, material=1, model="tabata",
  cartesian_product=False)`

### `pyamtrack.proton_models`

- `dose_bortfeld(depth_cm, fluence_cm2, energy_MeV,
  energy_spread_fraction=0.01, material=1, eps=0.03,
  cartesian_product=False)`
- `let_wilkens(depth_cm, energy_MeV, energy_spread_fraction=0.01,
  material=1, averaging="dose", cartesian_product=False)`

### `pyamtrack.track`

- `track_width(energy_MeV, material=1, model="tabata",
  cartesian_product=False)`
- `katz_point`, `geiss`, `katz_site`, `cucinotta_point`,
  `katz_extended_target`, and `cucinotta_extended_target`, each with the
  signature `(radius_m, energy_MeV_u, particle, material=1,
  rdd_parameters=None, electron_range_model=<model default>,
  source="default", cartesian_product=False, allow_bethe_fallback=False)`.

There is one radial dose function per radial dose distribution (RDD) model
instead of a model selector, so the RDD model IDs are not part of the Python
API. The libamtrack test RDD, the radical-diffusion RDD, shell-averaged doses,
and the inverse (radius from dose) calculation are not exposed yet.

| Function | `rdd_parameters` keys | Default `electron_range_model` |
| --- | --- | --- |
| `katz_point` | `r_min_m`, `d_min_Gy` | `"waligorski"` |
| `geiss` | `a0_m` | `"geiss"` |
| `katz_site` | `a0_m`, `d_min_Gy` | `"waligorski"` |
| `cucinotta_point` | `r_min_m`, `d_min_Gy` | `"tabata"` |
| `katz_extended_target` | `r_min_m`, `a0_m`, `d_min_Gy` | `"waligorski"` |
| `cucinotta_extended_target` | `r_min_m`, `a0_m`, `d_min_Gy` | `"tabata"` |

The radial dose functions accept `radius_m` as a scalar, list, or 1-D NumPy
array. `energy_MeV_u`, `particle`, and `material` define the evaluated
configurations and follow the usual elementwise or `cartesian_product`
semantics. The result has shape `(*configuration_shape, n_radii)`; the radius
axis is omitted for a scalar `radius_m`, and a float is returned when every
argument is scalar.

`get_ids`, `get_long_names`, `get_names`, `get_acronyms`, and `get_models`
take no arguments. `Averaging` is an enum, not a function argument, and its
current values are `proton_models.Averaging.DOSE` and
`proton_models.Averaging.TRACK`.

## Argument dictionary

This dictionary is organized by the public Python argument name. “Used by”
lists the currently bound functions or constructors that use the argument.

### `A`

- **Meaning:** Nuclear mass number.
- **Type:** Integer.
- **Used by:** `particles.from_ZA`.
- **Rules:** Must identify a supported isotope together with `Z`.

### `Z`

- **Meaning:** Atomic number.
- **Type:** Integer.
- **Used by:** `particles.from_ZA`.
- **Rules:** Must identify a supported element together with `A`.

### `averaging`

- **Meaning:** LET averaging convention.
- **Type:** A case-insensitive `"dose"` or `"track"` string, or a
  `proton_models.Averaging` enum value.
- **Used by:** `proton_models.let_wilkens`.
- **Default:** `"dose"`.
- **Rules:** `"dose"` selects dose-averaged LET (`LET_d`); `"track"` selects
  track-averaged LET (`LET_t`). Invalid strings raise `ValueError`.

### `beta`

- **Meaning:** Particle speed divided by the speed of light, `v/c`.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `converters.energy_from_beta`.
- **Units:** Dimensionless.

### `cartesian_product`

- **Meaning:** Whether iterable arguments should be combined across every
  possible combination.
- **Type:** Boolean.
- **Used by:** `stopping.electron_range`,
  `proton_models.dose_bortfeld`, `proton_models.let_wilkens`, and the
  `track` functions.
- **Default:** `False`.
- **Rules:** `False` evaluates values elementwise and broadcasts scalar
  arguments. `True` evaluates the Cartesian product and returns an array
  shaped by the iterable arguments.

### `depth_cm`

- **Meaning:** Depth in the material.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `proton_models.dose_bortfeld` and
  `proton_models.let_wilkens`.
- **Units and validation:** Centimetres; every value must be non-negative.

### `energy_MeV`

- **Meaning:** Initial kinetic energy.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `stopping.electron_range`,
  `proton_models.dose_bortfeld`, and `proton_models.let_wilkens`.
- **Units and validation:** MeV. The proton models require values in
  `[0.1, 10000.0]` MeV.

### `electron_range_model`

- **Meaning:** Delta-electron range model used inside a radial dose
  distribution.
- **Type:** A model name from `stopping.get_models()`, a model ID, or a
  `stopping.ElectronRangeModel` value. Python `bool` values are rejected.
- **Used by:** the `track` radial dose functions.
- **Rules:** The Katz RDDs (`katz_point`, `katz_site`,
  `katz_extended_target`) accept only `"butts_katz"`, `"waligorski"`, and
  `"edmund"`; other combinations raise `ValueError` instead of returning
  zero dose. The default is function-specific.

### `energy_MeV_u`

- **Meaning:** Kinetic energy per nucleon.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `converters.beta_from_energy` and the `track` radial dose
  functions.
- **Units:** MeV/u, where `u` is the atomic mass unit.
- **Rules (`track`):** Must be finite, in `(0, 1e6)`, and inside the energy
  range of the selected stopping-power source.

### `energy_spread_fraction`

- **Meaning:** Relative energy spread, interpreted as a fraction of
  `energy_MeV`.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `proton_models.dose_bortfeld` and
  `proton_models.let_wilkens`.
- **Default:** `0.01` (one percent).
- **Rules:** Must be strictly between `0` and `1`. The implementation
  converts it to an absolute spread in MeV by multiplying it by
  `energy_MeV`.

### `eps`

- **Meaning:** Fraction of primary fluence assigned to the nuclear
  interaction tail.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `proton_models.dose_bortfeld`.
- **Default:** `0.03`.
- **Rules:** Must be in `[0, 1)`. It is dimensionless.

### `fluence_cm2`

- **Meaning:** Proton fluence.
- **Type:** Scalar number, Python list, or NumPy array.
- **Used by:** `proton_models.dose_bortfeld`.
- **Units:** `1/cm²` (inverse square centimetres). Keep this established
  public name, and clarify the reciprocal unit in documentation.

### `id`

- **Meaning:** Material identifier.
- **Type:** Integer.
- **Used by:** `materials.Material(id)`.
- **Rules:** Use an ID returned by `pyamtrack.materials.get_ids()`.
  `valid_material_ids` is not an exposed API attribute. Material ID `1` is
  liquid water and is the default for the calculation functions that accept
  `material`.

### `material`

- **Meaning:** Material in which the calculation is performed.
- **Type:** A material ID, a `pyamtrack.materials.Material` object, or a
  supported list/NumPy array of those values. Python `bool` values are not
  accepted as material IDs.
- **Used by:** `stopping.electron_range`,
  `proton_models.dose_bortfeld`, `proton_models.let_wilkens`, and the
  `track` functions.
- **Default:** `1` (liquid water).
- **Rules:** IDs are validated against the material table. A `Material`
  object is converted to its `id` before calculation. Sequences are
  validated element by element. Boolean values are rejected rather than
  treated as integer IDs.

### `model`

- **Meaning:** Selectable calculation model.
- **Type:** A function-specific model name (`str`), model ID (`int`), enum
  value, or supported list/NumPy array of model selectors.
- **Used by:** `stopping.electron_range` and any future API that exposes
  multiple calculation models.
- **Rules:** Accepted names, IDs, enum values, and defaults are defined by
  each function. Use `model` when the argument selects the implementation or
  theoretical model used for a calculation.

### `name`

- **Meaning:** A domain-specific string name.
- **Type:** String.
- **Used by:** `stopping.model(name)`, `materials.Material(name)`, and
  `particles.from_string(name)`.
- **Rules:** The accepted values depend on the function: stopping-model
  names, material names, or element/isotope/special-particle labels,
  respectively. Do not use `name` for an ID or a free-form description.

### `radius_m`

- **Meaning:** Radial distance from the ion path.
- **Type:** Scalar number, Python list, or 0-D/1-D NumPy array.
- **Used by:** the `track` radial dose functions.
- **Units and validation:** Metres; every value must be finite and
  non-negative. A sequence forms the last axis of the result. Radii below the
  model's lower cut-off give zero dose.

### `rdd_parameters`

- **Meaning:** Model-specific radial dose distribution parameters.
- **Type:** `dict` mapping parameter names to numbers, or `None`.
- **Used by:** the `track` radial dose functions.
- **Default:** `None` (libamtrack defaults for the model).
- **Rules:** Only the keys listed for the function are accepted; omitted
  keys keep their default. `r_min_m` and `a0_m` must be positive and
  `d_min_Gy` must be non-negative.

### `pdg_code`

- **Meaning:** Particle Data Group code.
- **Type:** Integer.
- **Used by:** `particles.Particle(pdg_code)`,
  `particles.ions.Ion(pdg_code)`, and `particles.from_pdg(pdg_code)`.
- **Rules:** `Particle` accepts non-ion particle codes; `Ion` accepts nuclear
  ion codes. `from_pdg` returns the appropriate `Particle` or `Ion`.

## Adding or changing a bound argument

When exposing a new function:

1. Choose a stable Python name using the rules above.
2. Declare it explicitly with `nb::arg("argument_name")`, especially when
   keyword calls should be supported.
3. Put required arguments before optional arguments and give defaults in the
   binding, not only in the C++ declaration.
4. Use the same name in the nanobind docstring, the Python API inventory, and
   this dictionary.
5. Document units, accepted scalar/container types, defaults, validation,
   broadcasting, and output shape.
6. Add a test for the positional call and, when declared, the keyword call.

If an argument is renamed, treat it as a Python API change: update all
`nb::arg` declarations, docstrings, tests, examples, and this document
together.
