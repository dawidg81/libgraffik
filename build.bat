@echo off
REM build.bat - Batch build script for libgraffik
REM Usage: build.bat <backend> <example>

setlocal enabledelayedexpansion

set LIB_DIR=lib
set EXAMPLES_DIR=examples
set BUILD_DIR=build
set CXX=g++
set CXXFLAGS=-std=c++11 -Wall -Wextra -I%LIB_DIR%

if "%1"=="" goto show_help
if "%1"=="help" goto show_help
if "%1"=="-h" goto show_help
if "%1"=="--help" goto show_help
if "%1"=="clean" goto clean

set BACKEND=%1
set EXAMPLE=%2

if "%EXAMPLE%"=="" (
    echo Error: Example not specified
    echo Usage: build.bat ^<backend^> ^<example^>
    echo Example: build.bat sdl sample1
    exit /b 1
)

REM Validate example exists
if not exist "%EXAMPLES_DIR%\%EXAMPLE%.cpp" (
    echo Error: Example '%EXAMPLE%' not found
    echo.
    echo Available examples:
    for %%f in (%EXAMPLES_DIR%\*.cpp) do echo   %%~nf
    exit /b 1
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Set backend-specific flags
if /i "%BACKEND%"=="sdl" (
    set BACKEND_DEFINE=-DUSE_SDL
    if not defined SDL2_PATH set SDL2_PATH=C:\SDL2
    set INCLUDES=-I!SDL2_PATH!\include -I!SDL2_PATH!\include\SDL2
    set LDFLAGS=-L!SDL2_PATH!\lib
    set LIBS=-lmingw32 -lSDL2main -lSDL2 -mwindows
    
    REM Copy SDL2.dll
    if exist "%LIB_DIR%\SDL2.dll" (
        copy /y "%LIB_DIR%\SDL2.dll" "%BUILD_DIR%\" >nul 2>&1
    ) else if exist "!SDL2_PATH!\bin\SDL2.dll" (
        copy /y "!SDL2_PATH!\bin\SDL2.dll" "%BUILD_DIR%\" >nul 2>&1
    )
) else if /i "%BACKEND%"=="win32" (
    set BACKEND_DEFINE=-DUSE_WIN32
    set INCLUDES=
    set LDFLAGS=
    set LIBS=-lgdi32 -luser32
) else (
    echo Error: Invalid backend '%BACKEND%'
    echo Available backends: sdl, win32
    exit /b 1
)

echo Building %EXAMPLE% with %BACKEND% backend...
echo.

REM Build command
%CXX% %CXXFLAGS% %BACKEND_DEFINE% %INCLUDES% ^
    %EXAMPLES_DIR%\%EXAMPLE%.cpp %LIB_DIR%\graphics.cpp ^
    -o %BUILD_DIR%\%EXAMPLE%.exe ^
    %LDFLAGS% %LIBS%

if %errorlevel% equ 0 (
    echo.
    echo Build successful!
    echo Output: %BUILD_DIR%\%EXAMPLE%.exe
    echo.
    echo Run with: %BUILD_DIR%\%EXAMPLE%.exe
    exit /b 0
) else (
    echo.
    echo Build failed!
    exit /b 1
)

:show_help
echo ========================================
echo    libgraffik Build System
echo ========================================
echo.
echo Usage:
echo   build.bat ^<backend^> ^<example^>
echo   build.bat clean
echo.
echo Available Backends:
echo   sdl    - SDL2 (cross-platform)
echo   win32  - Win32 API (Windows native)
echo.
echo Available Examples:
for %%f in (%EXAMPLES_DIR%\*.cpp) do echo   %%~nf
echo.
echo Examples:
echo   build.bat sdl sample1
echo   build.bat win32 sample2
echo   build.bat clean
echo.
exit /b 0

:clean
echo Cleaning build files...
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo Clean complete
) else (
    echo Nothing to clean
)
exit /b 0
