# Building and Installing `pyamtrack` on Linux

## **Prerequisites**
Ensure you have the following installed:
- **Python 3** (with development headers, at least 3.9)
- **CMake** (3.16 or later)
- **Ninja** (recommended for faster builds)
- **GSL** (GNU Scientific Library)
- **Git** (for fetching dependencies)

## **Installation Instructions**

Follow these steps to build and install the `pyamtrack` Python module as a wheel package.

### **Building and Installing the Project Locally**

To build and install the project locally, follow these steps:

1. **Create a Virtual Environment**:
   Create and activate a Python virtual environment:
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   ```

2. **Install Development Dependencies**:
   Install the required dependencies for building the project:
   ```bash
   pip install -r requirements-build.txt
   ```

3. **Build the Project**:
   Run the following command to build the project:
   ```bash
   python -m build --wheel --no-isolation --config-setting=build-dir=./build
   ```

4. **Install the Built Wheel**:
   After building, install the wheel:
   ```bash
   pip install dist/*.whl
   ```

5. **Test the Installation**:
   Verify the installation by running:
   ```bash
   python -c "import pyamtrack; print(pyamtrack.converters.beta_from_energy(150))"
   ```

Expected output:
```
Python bindings for libamtrack
```

## **Uninstallation**
### **Remove the Installed Package**
```sh
pip uninstall pyamtrack
```

## **Rebuilding the Package**
### **Clean Previous Builds**
```sh
rm -rf build dist
```

### **Rebuild the Wheel**
```sh
python -m build --wheel
```

Now you can use `pyamtrack` in your Python scripts:
```python
import pyamtrack
print(pyamtrack.converters.beta_from_energy(150))
```

### Building with cibuildwheel (on Linux)

The command below downloads and compiles GSL, it may take much longer.
```sh
pip install cibuildwheel
```

```sh
cibuildwheel --only "cp312-manylinux_x86_64"
```

To test installed package type in `venv`:

```sh
pip install wheelhouse/*whl
```

Then

```python
>>> import pyamtrack
>>> pyamtrack.electron_range(120)
0.0006700734300203331
>>> pyamtrack.__version__
'0.14.1'
>>> 
```