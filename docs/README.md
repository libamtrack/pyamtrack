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
```sh
python -m build
```

### **5. Install the Built Wheel**
```sh
pip install dist/*.whl
```

### **6. Test the Python Module**
```sh
python -c "import pyamtrack; print(pyamtrack.__doc__)"
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
python -m build
```

Now you can use `pyamtrack` in your Python scripts:
```python
import pyamtrack
print(pyamtrack.beta_from_energy(150))
```

