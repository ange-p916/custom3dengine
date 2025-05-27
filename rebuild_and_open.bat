@echo off
setlocal

REM --- Configuration ---
SET BUILD_DIR_NAME=build
SET CMAKE_GENERATOR_NAME=Visual Studio 17 2022
REM For CMAKE_GENERATOR_NAME, we will add quotes when it's USED, not when it's SET,
REM because the -G flag in CMake expects the generator name itself,
REM and if it has spaces, the whole thing needs to be quoted on the command line.

SET SOLUTION_FILE_NAME=SimpleEngine.sln
REM --- End Configuration ---

echo --- Cleaning up previous build directory ---
if exist "%BUILD_DIR_NAME%" (
    echo Deleting %BUILD_DIR_NAME% directory...
    rmdir /S /Q "%BUILD_DIR_NAME%"
    if errorlevel 1 (
        echo Failed to delete %BUILD_DIR_NAME%. It might be in use.
        echo Please close any programs using files in this directory (like Visual Studio or your running app).
        pause
        exit /b 1
    )
    echo Old build directory deleted.
    timeout /t 2 /nobreak > nul
) else (
    echo No existing %BUILD_DIR_NAME% directory to delete.
)

echo.
echo --- Creating new build directory ---
mkdir "%BUILD_DIR_NAME%"
if errorlevel 1 (
    echo Failed to create %BUILD_DIR_NAME% directory.
    pause
    exit /b 1
)
cd "%BUILD_DIR_NAME%"
if errorlevel 1 (
    echo Failed to change directory to %BUILD_DIR_NAME%.
    pause
    exit /b 1
)

echo.
echo --- Running CMake to generate Visual Studio solution ---
echo Using generator: %CMAKE_GENERATOR_NAME%
REM Note the quotes around "%CMAKE_GENERATOR_NAME%" when it's used
cmake .. -G "%CMAKE_GENERATOR_NAME%"
if errorlevel 1 (
    echo CMake configuration failed.
    pause
    exit /b 1
)
echo CMake configuration successful.

echo.
timeout /t 2 /nobreak > nul

echo --- Opening Visual Studio Solution ---
if exist "%SOLUTION_FILE_NAME%" (
    echo Opening %SOLUTION_FILE_NAME%...
    start "" "%SOLUTION_FILE_NAME%"
) else (
    echo ERROR: Solution file "%SOLUTION_FILE_NAME%" not found in %BUILD_DIR_NAME% directory!
    echo CMake might have failed to generate it, or the project name is different.
    pause
)

echo.
echo Script finished.
cd ..
endlocal