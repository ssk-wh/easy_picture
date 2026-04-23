@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul 2>&1

set BUILD_DIR=build
set BUILD_TYPE=Release
set FORCE_CLEAN=0
set CMAKE_OPTS=

REM ---- Parse arguments ----
if "%~1"=="debug"   set BUILD_TYPE=Debug
if "%~1"=="release" set BUILD_TYPE=Release
if "%~1"=="clean" (
    if exist %BUILD_DIR% (
        echo Cleaning build directory...
        rmdir /s /q %BUILD_DIR%
    )
    echo Cleaned.
    exit /b 0
)
if "%~1"=="--clean" set FORCE_CLEAN=1

echo ================================================
echo   SimplePicture Build [%BUILD_TYPE%]
echo ================================================
echo.

REM ---- Find MinGW64 ----
set MINGW64=C:\msys64\mingw64\bin
if not exist "%MINGW64%\cmake.exe" (
    echo [ERROR] MinGW64 not found at %MINGW64%
    echo         Please install MSYS2 and run:
    echo         pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-qt5-base mingw-w64-x86_64-qt5-svg mingw-w64-x86_64-qt5-imageformats
    exit /b 1
)
set "PATH=%MINGW64%;%PATH%"
echo [+] MinGW64: %MINGW64%
echo.

REM ---- Clean previous build ----
if exist %BUILD_DIR% (
    echo Cleaning previous build...
    rmdir /s /q %BUILD_DIR%
)

REM ---- CMake configure ----
echo [1/2] CMake configure...
cmake -S . -B %BUILD_DIR% -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DBUILD_TESTS=OFF %CMAKE_OPTS%
if errorlevel 1 (
    echo [ERROR] CMake configure failed.
    exit /b 1
)
echo.

REM ---- Build ----
echo [2/2] Building...
cmake --build %BUILD_DIR% --config %BUILD_TYPE%
if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)
echo.

REM ---- Verify output ----
if exist %BUILD_DIR%\SimplePicture.exe (
    for %%A in (%BUILD_DIR%\SimplePicture.exe) do set EXE_SIZE=%%~zA
    echo ================================================
    echo   Build succeeded: %BUILD_DIR%\SimplePicture.exe
    echo   Size: !EXE_SIZE! bytes
    echo ================================================
    exit /b 0
) else (
    echo [ERROR] Build completed but executable not found!
    echo         Expected: %BUILD_DIR%\SimplePicture.exe
    exit /b 1
)
