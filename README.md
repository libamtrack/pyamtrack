mkdir build
cd build

cmake .. -DGSL_INCLUDE_DIR="../vcpkg_installed/x64-windows/include" -DGSL_LIBRARY="../vcpkg_installed/x64-windows/lib/gsl.lib" -DGSL_CBLAS_LIBRARY="../vcpkg_installed/x64-windows/lib/gslcblas.lib" -DGETOPT_LIBRARY="../vcpkg_installed/x64-windows/lib/getopt.lib"  -DGETOPT_INCLUDE_DIR="../vcpkg_installed/x64-windows/include" -G "Ninja"

cmake .. -DGSL_INCLUDE_DIR="../vcpkg_installed/x64-windows/include" -DGSL_LIBRARY="../vcpkg_installed/x64-windows/lib/gsl.lib" -DGSL_CBLAS_LIBRARY="../vcpkg_installed/x64-windows/lib/gslcblas.lib" -G "Ninja"


cmake --build . --config Release
