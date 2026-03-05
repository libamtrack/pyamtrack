# Set up directories
$workspace = "$PWD"
$vcpkgCache = "$workspace\vcpkg_cache"
$vcpkgInstalled = "$workspace\vcpkg_installed"

if (-Not (Test-Path $vcpkgCache)) { New-Item -ItemType Directory -Path $vcpkgCache }
if (-Not (Test-Path $vcpkgInstalled)) { New-Item -ItemType Directory -Path $vcpkgInstalled }

# Clear stale registry cache
$registryCache = "$env:LOCALAPPDATA\vcpkg\registries"
if (Test-Path $registryCache) {
    Write-Output "Clearing vcpkg registry cache..."
    Remove-Item -Recurse -Force $registryCache
}

# Restore or install vcpkg
if (-Not (Test-Path "$workspace\vcpkg")) {
    git clone --depth 1 https://github.com/microsoft/vcpkg "$workspace\vcpkg"
    & "$workspace\vcpkg\bootstrap-vcpkg.bat" -disableMetrics
}

# Install dependencies using vcpkg
$env:VCPKG_DEFAULT_BINARY_CACHE = $vcpkgCache
$env:VCPKG_INSTALLED_DIR = "$workspace/vcpkg_installed"
& "$workspace\vcpkg\vcpkg.exe" install

Write-Output "VCPKG setup completed."
