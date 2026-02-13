# build.ps1 - PowerShell build script for libgraffik
# Usage: .\build.ps1 [options]

param(
    [string]$Backend = "",
    [string]$Example = "",
    [string]$Action = "build",
    [switch]$Help,
    [switch]$Clean,
    [switch]$All,
    [switch]$Run
)

$LIB_DIR = "lib"
$EXAMPLES_DIR = "examples"
$BUILD_DIR = "build"
$CXX = "g++"
$CXXFLAGS = "-std=c++11 -Wall -Wextra -I$LIB_DIR"

# Colors
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Show-Help {
    Write-Host ""
    Write-ColorOutput Cyan "╔════════════════════════════════════════╗"
    Write-ColorOutput Cyan "║    libgraffik Build System (PS)        ║"
    Write-ColorOutput Cyan "╚════════════════════════════════════════╝"
    Write-Host ""
    Write-ColorOutput Yellow "Usage:"
    Write-Host "  .\build.ps1 -Backend <backend> -Example <example>"
    Write-Host "  .\build.ps1 -Backend <backend> -All"
    Write-Host "  .\build.ps1 -Clean"
    Write-Host ""
    Write-ColorOutput Yellow "Available Backends:"
    Write-Host "  sdl    - SDL2 (cross-platform)"
    Write-Host "  win32  - Win32 API (Windows native)"
    Write-Host ""
    Write-ColorOutput Yellow "Available Examples:"
    Get-ChildItem -Path $EXAMPLES_DIR -Filter "*.cpp" | ForEach-Object {
        Write-Host "  $($_.BaseName)"
    }
    Write-Host ""
    Write-ColorOutput Yellow "Examples:"
    Write-Host "  .\build.ps1 -Backend sdl -Example sample1"
    Write-Host "  .\build.ps1 -Backend win32 -Example sample2"
    Write-Host "  .\build.ps1 -Backend sdl -Example sample1 -Run"
    Write-Host "  .\build.ps1 -Backend sdl -All"
    Write-Host "  .\build.ps1 -Clean"
    Write-Host ""
}

function Build-Example {
    param(
        [string]$Backend,
        [string]$Example
    )
    
    # Validate backend
    if ($Backend -notin @("sdl", "win32")) {
        Write-ColorOutput Red "Error: Invalid backend '$Backend'"
        Write-Host "Available backends: sdl, win32"
        exit 1
    }
    
    # Validate example
    $examplePath = Join-Path $EXAMPLES_DIR "$Example.cpp"
    if (-not (Test-Path $examplePath)) {
        Write-ColorOutput Red "Error: Example '$Example' not found"
        Write-Host "Available examples:"
        Get-ChildItem -Path $EXAMPLES_DIR -Filter "*.cpp" | ForEach-Object {
            Write-Host "  $($_.BaseName)"
        }
        exit 1
    }
    
    # Create build directory
    if (-not (Test-Path $BUILD_DIR)) {
        New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    }
    
    # Set backend-specific flags
    $backendDefine = ""
    $includes = ""
    $ldflags = ""
    $libs = ""
    
    if ($Backend -eq "sdl") {
        $backendDefine = "-DUSE_SDL"
        $SDL2_PATH = if ($env:SDL2_PATH) { $env:SDL2_PATH } else { "C:\SDL2" }
        $includes = "-I$SDL2_PATH\include -I$SDL2_PATH\include\SDL2"
        $ldflags = "-L$SDL2_PATH\lib"
        $libs = "-lmingw32 -lSDL2main -lSDL2 -mwindows"
        
        # Copy SDL2.dll if needed
        $sdlDllSource = if (Test-Path "$LIB_DIR\SDL2.dll") {
            "$LIB_DIR\SDL2.dll"
        } elseif (Test-Path "$SDL2_PATH\bin\SDL2.dll") {
            "$SDL2_PATH\bin\SDL2.dll"
        } else {
            $null
        }
        
        if ($sdlDllSource -and -not (Test-Path "$BUILD_DIR\SDL2.dll")) {
            Copy-Item $sdlDllSource "$BUILD_DIR\SDL2.dll"
            Write-ColorOutput Cyan "✓ SDL2.dll copied to build directory"
        }
    }
    elseif ($Backend -eq "win32") {
        $backendDefine = "-DUSE_WIN32"
        $libs = "-lgdi32 -luser32"
    }
    
    Write-ColorOutput Green "Building $Example with $Backend backend..."
    
    # Build command
    $libSource = Join-Path $LIB_DIR "graphics.cpp"
    $outputExe = Join-Path $BUILD_DIR "$Example.exe"
    
    $buildCmd = "$CXX $CXXFLAGS $backendDefine $includes $examplePath $libSource -o $outputExe $ldflags $libs"
    
    Write-Host "Command: $buildCmd"
    Invoke-Expression $buildCmd
    
    if ($LASTEXITCODE -eq 0) {
        Write-ColorOutput Green "✓ Build successful!"
        Write-Host "Output: $outputExe"
        return $true
    } else {
        Write-ColorOutput Red "✗ Build failed!"
        return $false
    }
}

function Clean-BuildFiles {
    Write-ColorOutput Yellow "Cleaning build files..."
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR
        Write-ColorOutput Green "✓ Clean complete"
    } else {
        Write-Host "Nothing to clean"
    }
}

# Main script logic
if ($Help) {
    Show-Help
    exit 0
}

if ($Clean) {
    Clean-BuildFiles
    exit 0
}

if ($All) {
    if (-not $Backend) {
        Write-ColorOutput Red "Error: -Backend required when using -All"
        Write-Host "Usage: .\build.ps1 -Backend sdl -All"
        exit 1
    }
    
    Write-ColorOutput Green "Building all examples with $Backend backend..."
    $success = $true
    
    Get-ChildItem -Path $EXAMPLES_DIR -Filter "*.cpp" | ForEach-Object {
        Write-Host ""
        if (-not (Build-Example -Backend $Backend -Example $_.BaseName)) {
            $success = $false
        }
    }
    
    Write-Host ""
    if ($success) {
        Write-ColorOutput Green "✓ All examples built successfully!"
    } else {
        Write-ColorOutput Red "✗ Some builds failed"
        exit 1
    }
    exit 0
}

if (-not $Backend -or -not $Example) {
    Write-ColorOutput Red "Error: -Backend and -Example required"
    Write-Host ""
    Show-Help
    exit 1
}

$success = Build-Example -Backend $Backend -Example $Example

if ($success -and $Run) {
    Write-Host ""
    Write-ColorOutput Green "Running $Example..."
    $exePath = Join-Path $BUILD_DIR "$Example.exe"
    & $exePath
}

exit $(if ($success) { 0 } else { 1 })
