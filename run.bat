@echo off
:: =============================================================================
:: run.bat — Install dependencies (if needed) and launch the robot controller.
::
:: Usage:
::   run.bat [OPTIONS]
::
:: Options:
::   --reinstall   Delete the install stamp and re-run the full installation
::   --help        Show this help message
::   --port <PORT> Choose the port to use for running the application
:: =============================================================================

setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"
set "STAMP=%PROJECT_ROOT%\scripts\.installed"

:: Argument parsing:
:: Add new flags in the if block below. Each flag should set a variable, then
:: the corresponding logic lives in the "Apply parsed options" section.

set "OPT_REINSTALL=0"
set "OPT_PORT=8090"

:parse_args
if "%~1"=="" goto end_args

if /i "%~1"=="--reinstall" (
    set "OPT_REINSTALL=1"
    shift
    goto parse_args
)

if /i "%~1"=="--port" (
    if "%~2"=="" (
        echo [run] --port requires a value 1>&2
        goto show_usage_error
    )
    set "OPT_PORT=%~2"
    shift & shift
    goto parse_args
)

if /i "%~1"=="--help" (
    goto show_usage
)

:: Add new options here:

echo [run] Unknown argument: %~1 1>&2
echo.
goto show_usage_error

:end_args

:: Apply parsed options
if "%OPT_REINSTALL%"=="1" (
    if exist "%STAMP%" (
        echo [run] --reinstall: removing install stamp...
        del "%STAMP%"
    ) else (
        echo [run] --reinstall: stamp not found, will run fresh install.
    )
)

:: Install (skipped automatically when stamp exists)
call "%PROJECT_ROOT%\scripts\install.bat"
if errorlevel 1 (
    echo [run] Installation failed. Aborting.
    exit /b 1
)

:: Launch
echo [run] Starting robot controller on port %OPT_PORT%...
"%PROJECT_ROOT%\.venv\Scripts\python.exe" "%PROJECT_ROOT%\src\control\main.py" --port "%OPT_PORT%"
exit /b 0

:: Help
:show_usage
echo Usage: run.bat [OPTIONS]
echo.
echo Options:
echo   --reinstall       Remove the install stamp and reinstall all dependencies
echo   --port ^<port^>   Port to run the web UI on (default: 8090)
echo   --help            Show this help message
exit /b 0

:show_usage_error
echo Usage: run.bat [OPTIONS]
echo.
echo Options:
echo   --reinstall       Remove the install stamp and reinstall all dependencies
echo   --port ^<port^>   Port to run the web UI on (default: 8090)
echo   --help            Show this help message
exit /b 1
