@echo off
REM Build script for TUI Editor using Visual Studio 2022 MSVC
REM Project structure:
REM   src/      - Source files (.c)
REM   include/  - Header files (.h)
REM   bin/      - Output executables
REM   obj/      - Object files

setlocal enabledelayedexpansion

REM Set Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" 2>nul
)
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" 2>nul
)

REM Compiler settings
set "CC=cl"
set "CFLAGS=/W4 /std:c11 /O2 /I include"
set "LDFLAGS=kernel32.lib"
set "TARGET=bin\editor.exe"

REM Source files
set "SOURCES=src\main.c src\editor.c src\display.c src\input.c src\file_ops.c src\utils.c"
set "OBJ_DIR=obj"

REM Parse command line
if "%1"=="" goto build
if "%1"=="build" goto build
if "%1"=="run" goto run
if "%1"=="clean" goto clean
if "%1"=="help" goto help
goto invalid

REM ==================== BUILD ====================
:build
echo [*] Building TUI Editor with MSVC...
echo [*] Source directory: src/
echo [*] Header directory: include/
echo [*] Output: %TARGET%

REM Create directories if they don't exist
if not exist bin mkdir bin
if not exist %OBJ_DIR% mkdir %OBJ_DIR%

REM Compile
%CC% %CFLAGS% %SOURCES% /Fo%OBJ_DIR%\ /link %LDFLAGS% /OUT:%TARGET%

if errorlevel 1 (
    echo.
    echo [!] Build FAILED!
    echo [!] Check errors above
    exit /b 1
)

echo.
echo [+] Build SUCCESSFUL!
echo [+] Output: %TARGET%
exit /b 0

REM ==================== RUN ====================
:run
echo [*] Building and running TUI Editor...

if not exist bin mkdir bin
if not exist %OBJ_DIR% mkdir %OBJ_DIR%

%CC% %CFLAGS% %SOURCES% /Fo%OBJ_DIR%\ /link %LDFLAGS% /OUT:%TARGET%

if errorlevel 1 (
    echo.
    echo [!] Build FAILED!
    exit /b 1
)

echo [+] Build successful!
echo [*] Running %TARGET%...
echo.
%TARGET%
exit /b 0

REM ==================== CLEAN ====================
:clean
echo [*] Cleaning build artifacts...

if exist bin (
    echo [*] Removing bin directory...
    rmdir /s /q bin
)

if exist %OBJ_DIR% (
    echo [*] Removing obj directory...
    rmdir /s /q %OBJ_DIR%
)

if exist *.ilk (
    echo [*] Removing .ilk files...
    del *.ilk
)

if exist *.pdb (
    echo [*] Removing .pdb files...
    del *.pdb
)

echo [+] Clean complete!
exit /b 0

REM ==================== HELP ====================
:help
echo.
echo ============================================
echo   TUI Editor - Build Script (MSVC)
echo ============================================
echo.
echo Usage: build.bat [command]
echo.
echo Commands:
echo   build    - Compile the project (default)
echo   run      - Compile and run the editor
echo   clean    - Remove all build artifacts
echo   help     - Show this help message
echo.
echo Project Structure:
echo   src/     - Source files (.c)
echo   include/ - Header files (.h)
echo   bin/     - Output executables (auto-created)
echo   obj/     - Object files (auto-created)
echo.
echo Examples:
echo   build.bat              (build only)
echo   build.bat run          (build and run)
echo   build.bat clean        (clean artifacts)
echo.
exit /b 0

REM ==================== INVALID ====================
:invalid
echo [!] Invalid command: %1
echo.
echo Run 'build.bat help' for usage information
exit /b 1