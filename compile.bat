@echo off
setlocal ENABLEDELAYEDEXPANSION

REM === Argument Checking ===
if "%~1"=="" (
    echo.
    echo [!] Usage: compile.bat [release^|debug] [true^|false]
    echo.
    exit /b 1
)

if "%~2"=="" (
    echo.
    echo [!] Usage: compile.bat [release^|debug] [true^|false]
    echo.
    exit /b 1
)

set "BUILD_TYPE=%~1"
set "RUN_AFTER=%~2"

if /I not "%BUILD_TYPE%"=="release" if /I not "%BUILD_TYPE%"=="debug" (
    echo.
    echo [ERROR] Invalid build type: "%BUILD_TYPE%"
    echo         Must be either "release" or "debug"
    echo.
    exit /b 1
)

if /I not "%RUN_AFTER%"=="true" if /I not "%RUN_AFTER%"=="false" (
    echo.
    echo [ERROR] Invalid run option: "%RUN_AFTER%"
    echo         Must be either "true" or "false"
    echo.
    exit /b 1
)

REM === Set Directories and Flags ===
if /I "%BUILD_TYPE%"=="debug" (
    set "OUT_DIR=build\debug"
    set "CFLAGS=-g -O0 -Wall -Wextra"
    set "LDFLAGS=-g"
    echo [*] Build Mode: DEBUG
) else (
    set "OUT_DIR=build\release"
    set "CFLAGS=-O2"
    set "LDFLAGS="
    echo [*] Build Mode: RELEASE
)

if not exist "%OUT_DIR%" (
    echo [*] Creating output directory: %OUT_DIR%
    mkdir "%OUT_DIR%"
)

REM === Copy data directory ===
if exist "%OUT_DIR%\data" (
    echo [*] Deleting old data folder...
    rd /s /q "%OUT_DIR%\data"
)

if exist "project\data" (
    echo [*] Copying data files...
    xcopy /E /I /Y "project\data" "%OUT_DIR%\data" >nul
)




REM === Find .c files ===
set "SOURCES="
for /R ".\project\src" %%f in (*.c) do (
    set "SOURCES=!SOURCES! "%%f""
)

set "EXE=%OUT_DIR%\program.exe"

REM === Compile ===
echo.
echo ============================================
echo            Compiling Source Files           
echo ============================================
echo.

REM === Compile resource file ===
echo [*] Compiling icon resource...
windres "project\src\icon.rc" -O coff -o "%OUT_DIR%\icon.res"
if errorlevel 1 (
    echo.
    echo [X] Failed to compile icon resource.
    echo.
    exit /b 1
)


gcc %CFLAGS% !SOURCES! %LDFLAGS% "%OUT_DIR%\icon.res" -o "!EXE!"
if errorlevel 1 (
    echo.
    echo [X] Compilation failed.
    echo.
    exit /b 1
)


echo.
echo [v/] Compilation successful!
echo [*] Executable: "!EXE!"
echo.

REM === Run If Requested ===
if /I "%RUN_AFTER%"=="true" (

    echo ============================================
    echo               Executing Program            
    echo ============================================
    echo.

    if /I "%BUILD_TYPE%"=="debug" (
        start "GDB Debugger" cmd  /c  gdb -tui -ex "set pagination off" -ex run -ex quit --args "!EXE!"
    ) else (
        start "Program Execution" "!EXE!"
    )
)

endlocal
exit /b 0
