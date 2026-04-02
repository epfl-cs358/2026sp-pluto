@echo off
:: =============================================================================
:: install.bat — Set up the Python virtual environment and install dependencies.
::
:: Behaviour:
::   - Checks for Python 3.13 (opens download page if missing)
::   - Creates .venv at the project root
::   - Installs packages from requirements.txt
::   - Writes a stamp file (.installed) so subsequent runs skip heavy work
:: =============================================================================

setlocal enabledelayedexpansion

:: Paths:
:: %~dp0 includes a trailing backslash - strip it for clean concatenation
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
for %%I in ("%SCRIPT_DIR%\..") do set "PROJECT_ROOT=%%~fI"

set "STAMP=%SCRIPT_DIR%\.installed"
set "VENV=%PROJECT_ROOT%\.venv"
set "REQUIREMENTS=%PROJECT_ROOT%\requirements.txt"
set "DOWNLOAD_URL=https://www.python.org/downloads/"

:: Stamp check:
:: Stamp check:
set "CURRENT_HASH="
for /f "skip=1 tokens=* delims=" %%H in ('certutil -hashfile "%REQUIREMENTS%" SHA256 2^>nul') do (
    if not defined CURRENT_HASH set "CURRENT_HASH=%%H"
)

if exist "%STAMP%" (
    set /p STORED_HASH=<"%STAMP%"
    if "!CURRENT_HASH!"=="!STORED_HASH!" (
        echo [install] Already installed ^(requirements unchanged^). Skipping heavy operations.
        exit /b 0
    )
    echo [install] requirements.txt has changed. Reinstalling...
)

echo [install] Starting installation...

:: Python 3.13 detection:
:: Strategy 1: Python Launcher (py.exe)
set "PYTHON="
where py >nul 2>&1
if %errorlevel%==0 (
    py -3.13 --version >nul 2>&1
    if !errorlevel!==0 (
        set "PYTHON=py -3.13"
    )
)

:: Strategy 2: plain "python" on PATH - check its reported version
if not defined PYTHON (
    where python >nul 2>&1
    if !errorlevel!==0 (
        for /f "tokens=2 delims= " %%V in ('python --version 2^>^&1') do (
            echo %%V | findstr /b "3.13" >nul 2>&1
            if !errorlevel!==0 set "PYTHON=python"
        )
    )
)

:: Strategy 3: python3 alias (less common on Windows but possible via Store/WSL tools)
if not defined PYTHON (
    where python3 >nul 2>&1
    if !errorlevel!==0 (
        for /f "tokens=2 delims= " %%V in ('python3 --version 2^>^&1') do (
            echo %%V | findstr /b "3.13" >nul 2>&1
            if !errorlevel!==0 set "PYTHON=python3"
        )
    )
)

if not defined PYTHON (
    echo.
    echo   [ERROR] Python 3.13 was not found on this system.
    echo           Please download and install it from:
    echo           %DOWNLOAD_URL%
    echo.
    start "" "%DOWNLOAD_URL%"
    exit /b 1
)

for /f "tokens=*" %%V in ('!PYTHON! --version 2^>^&1') do echo [install] Found: %%V

:: Virtual environment:
if exist "%VENV%" (
    echo [install] Removing existing virtual environment...
    rmdir /s /q "%VENV%"
)

echo [install] Creating virtual environment at: %VENV%
!PYTHON! -m venv "%VENV%"
if errorlevel 1 (
    echo [ERROR] Failed to create virtual environment.
    exit /b 1
)

:: Dependencies:
echo [install] Upgrading pip...
"%VENV%\Scripts\python.exe" -m pip install --upgrade pip --quiet
if errorlevel 1 (
    echo [ERROR] Failed to upgrade pip.
    exit /b 1
)

echo [install] Installing requirements from: %REQUIREMENTS%
"%VENV%\Scripts\python.exe" -m pip install -r "%REQUIREMENTS%"
if errorlevel 1 (
    echo [ERROR] pip install failed. Check requirements.txt and your network connection.
    exit /b 1
)

:: Stamp:
echo !CURRENT_HASH!> "%STAMP%"
echo.
echo [install] Installation complete.
exit /b 0