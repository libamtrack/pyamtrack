# pyamtrack

pyamtrack is a Python wrapper for the libamtrack library, making it easier to access and use libamtrack's functionality directly from Python.

## Stable Release Information

The last stable release of pyamtrack is **0.14.0** (released on **2022-10-03**). However, this version has several limitations:
- It is only available as a binary wheel package for **Linux** (no support for Windows or macOS).
- There is no documentation describing the available functions or how to use them.

To install the stable release, run:
```bash
pip install pyamtrack
```

## Ongoing Development

A new version of pyamtrack is currently being developed, addressing these limitations. The new release will include:
- Support for **Linux, Windows, and macOS** (for Python >= 3.9).
- Full documentation detailing all available functions, their usage, and example applications.

The work is still in progress, and the latest **alpha release** can be installed via:
```bash
pip install pyamtrack --pre
```
*(Use at your own risk, as many things may still be broken.)*

### Usage Example

Here's a quick example demonstrating how to use pyamtrack (ongoing development release):
```python
from pyamtrack.converters import beta_from_energy

print(beta_from_energy(150))
```

### Documentation

Detailed user documentation is available at **[https://libamtrack.github.io/](https://libamtrack.github.io/)**. It includes:
- Instructions for installing the Python library.
- Guidelines on how to use pyamtrack.
- Descriptions of all available functions, including examples.

### Example Notebooks

For more detailed examples and tutorials, please refer to the notebooks in the `example` directory.
