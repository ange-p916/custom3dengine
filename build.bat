@echo off
SET BUILD_DIR=build
SET CMAKE_GENERATOR="Visual Studio 17 2022"  # Or your VS version

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
    echo --- Configuring CMake ---
    cmake -S . -B %BUILD_DIR% -G %CMAKE_GENERATOR%
    if %errorlevel% neq 0 (
        echo CMake configuration failed.
        exit /b %errorlevel%
    )
)

echo --- Building Project ---
cmake --build %BUILD_DIR% --config Debug  # Or Release
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)

echo --- Build Succeeded ---
REM Optional: Run the executable
REM %BUILD_DIR%/Debug/SimpleEngine.exe