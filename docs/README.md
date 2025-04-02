# Building and Installing `pyamtrack` on Linux

## **Prerequisites**
Ensure you have the following installed:
- **Python 3** (with development headers)
- **CMake** (3.16 or later)
- **Ninja** (recommended for faster builds)
- **GSL** (GNU Scientific Library)
- **Git** (for fetching dependencies)

## **Installation Instructions**

Follow these steps to build and install the `pyamtrack` Python module as a wheel package.

### **1. Create a Virtual Environment**
```sh
python3 -m venv venv
```

### **2. Activate the Virtual Environment**
```sh
source venv/bin/activate
```

### **3. Install Build Dependencies**
```sh
pip install build
```

### **4. Build the Wheel Package**

Clean all previously generated packages:

```sh
rm -rf dist/
```

The command below assumes that GSL is installed in your OS.
```sh
python -m build --wheel
```

### **5. Install the Built Wheel**
```sh
pip install dist/*.whl
```

### **6. Test the Python Module**
```sh
python -c "import pyamtrack; print(pyamtrack.beta_from_energy(150))"
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
print(pyamtrack.beta_from_energy(150))
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