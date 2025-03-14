# Building and Testing `pyamtrack` on Windows

## **Prerequisites**
Ensure you have the following installed:
- **CMake** (3.16 or later)
- **Ninja** build system
- **Python 3** (with development headers)
- **vcpkg** (for dependency management, including GSL)
- **Git** (for fetching dependencies)
- **A C++ Compiler** (MSVC, Clang, or MinGW)

## **Compilation Instructions**

Follow these steps to build and test the `pyamtrack` Python module:

### **1. Create and Enter the Build Directory**
```sh
mkdir build
cd build
```

### **2. Configure the Build with CMake**
```sh
cmake .. -DGSL_INCLUDE_DIR="../vcpkg_installed/x64-windows/include" -DGSL_LIBRARY="../vcpkg_installed/x64-windows/lib/gsl.lib" -DGSL_CBLAS_LIBRARY="../vcpkg_installed/x64-windows/lib/gslcblas.lib" -G "Ninja"
```

### **3. Build the Project**
```sh
cmake --build . --config Release --parallel
```

### **4. Set `PYTHONPATH` to the Built Module**
```sh
set PYTHONPATH=%cd%\Release;%PYTHONPATH%
```

### **5. Test the Python Module**
Run the following command to verify that the module is working:
```sh
python -c "import pyamtrack; print(pyamtrack.__doc__)"
```
Expected output:
```
Python bindings for libamtrack
```

## **Troubleshooting**
- If you get an `ImportError`, make sure:
  - The module was built in `build/Release`
  - `PYTHONPATH` is correctly set
  - You are using the correct Python version
- If `pyamtrack.pyd` is missing, try a **clean rebuild**:
  ```sh
  rmdir /s /q build  # Delete the build directory
  mkdir build && cd build
  cmake .. -G "Ninja"
  cmake --build . --config Release
  ```

## **Installation**
To permanently install the module:
```sh
cmake --install .
```

Now you can import `pyamtrack` from any Python script:
```python
import pyamtrack
print(pyamtrack.example_function(10))
```
