@echo off
setlocal enabledelayedexpansion

REM ===============================================
REM SimplePicture - Windows Pack Script
REM Auto-calls build_on_win.bat release at the start.
REM Usage: .\pack_on_win.bat  (no need to run build manually)
REM Requires MSYS2 for ldd (DLL collection) and makensis.
REM   pacman -S mingw-w64-x86_64-nsis zip
REM ===============================================

cd /d "%~dp0"

echo.
echo ================================================
echo   Step 1/3: Building Release...
echo ================================================
chcp.com 65001 >nul 2>&1
cmd /c ""%~dp0build_on_win.bat" release"
if errorlevel 1 (
    echo [ERROR] Build failed! Aborting pack.
    exit /b 1
)

if not exist "build\SimplePicture.exe" (
    echo [ERROR] build\SimplePicture.exe not found after build.
    exit /b 1
)

echo.
echo ================================================
echo   Step 2/3: Collecting DLLs...
echo ================================================

REM MSYS2 bash is required for ldd
set "BASH=C:\msys64\usr\bin\bash.exe"
if not exist "%BASH%" (
    echo [ERROR] MSYS2 not found. Install from https://www.msys2.org/
    echo         Then: pacman -S mingw-w64-x86_64-nsis zip
    exit /b 1
)

REM Convert project path to Unix format via temp file (avoids for/f quoting issues)
set "WINPATH=%~dp0"
if "!WINPATH:~-1!"=="\" set "WINPATH=!WINPATH:~0,-1!"
set "TMP_SH=%TEMP%\sp_cygpath.sh"
set "TMP_OUT=%TEMP%\sp_unixpath.txt"
echo cygpath -u "!WINPATH!" > "!TMP_SH!"
"%BASH%" -l "!TMP_SH!" > "!TMP_OUT!" 2>nul
set /p UNIX_DIR= < "!TMP_OUT!"
del "!TMP_SH!" "!TMP_OUT!" 2>nul

if "!UNIX_DIR!"=="" (
    echo [ERROR] Path conversion failed.
    exit /b 1
)

REM Run DLL collection + NSIS (--skip-build: build was already done above)
REM MSYSTEM=MINGW64 ensures bash login shell includes /mingw64/bin in PATH for ldd
set MSYSTEM=MINGW64
"%BASH%" -lc "cd '!UNIX_DIR!' && bash installer/build_installer.sh --skip-build"
if errorlevel 1 (
    echo [ERROR] DLL collection or NSIS packaging failed.
    exit /b 2
)

echo.
echo ================================================
echo   SUCCESS: Pack completed!
echo ================================================
echo.
echo Installer location:
dir installer\SimplePicture-*.exe
echo.
