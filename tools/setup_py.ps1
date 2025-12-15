#!/usr/bin/env pwsh

# This script is used to perform post-build actions for development.
# It installs the build, copies necessary .pdb and .dll files to the output directory,
# and copies Python tools from the vcpkg installation.

# Get project root directory
$PROJECT_ROOT = $(git rev-parse --show-toplevel)
if (!$PROJECT_ROOT) {
    Write-Error "Failed to determine project root directory"
    exit 1
}

$BUILD_ROOT = "$PROJECT_ROOT/build"

$BIN_DIR = "$BUILD_ROOT/out/bin/"
$PY_EXE = "$BIN_DIR/python.exe"
$REQUIREMENTS_TXT = "$PROJECT_ROOT/copilot/assistant/mcp/requirements.txt"

# Ensure bin directory exists
if (!(Test-Path $BIN_DIR)) {
    New-Item -ItemType Directory -Path $BIN_DIR -Force
}

# Check if python executable exists
if (!(Test-Path $PY_EXE)) {
    Write-Error "Python executable not found at $PY_EXE"
    exit 1
}

# Check if requirements file exists
if (!(Test-Path $REQUIREMENTS_TXT)) {
    Write-Error "Requirements file not found at $REQUIREMENTS_TXT"
    exit 1
}

# Ensure pip and install requirements
& $PY_EXE -m ensurepip
& $PY_EXE -m pip install -r $REQUIREMENTS_TXT