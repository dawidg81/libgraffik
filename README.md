# libgraffik - Cross-Platform Graphics Library

A simple cross-platform graphics abstraction layer with support for multiple backends (SDL2, Win32, X11).

## Implemented Backends

- ✅ **SDL2** (Windows, Linux, macOS) - Fully implemented
- ✅ **Win32** (Windows native) - Fully implemented
- ✅ **X11** (Linux native) - Fully implemented

## Features

- Window creation and management
- Double buffering for smooth rendering
- Basic drawing primitives:
  - Lines
  - Rectangles (filled and outlined)
  - Circles (filled and outlined)
  - Pixels
- Color support with alpha channel (SDL only)
- Simple event handling (ESC key, window close)
- Cross-platform delay function

## Building on Windows

### Option 1: Batch File (Easiest for Windows)

```cmd
REM Build with SDL2
build.bat sdl sample1

REM Build with Win32
build.bat win32 sample1

REM Clean
build.bat clean

REM Show help
build.bat help
```

### Option 2: PowerShell Script (Recommended for Windows)

```powershell
# Build with SDL2
.\build.ps1 -Backend sdl -Example sample1

# Build with Win32
.\build.ps1 -Backend win32 -Example sample2

# Build and run
.\build.ps1 -Backend sdl -Example sample1 -Run

# Build all examples
.\build.ps1 -Backend sdl -All

# Clean
.\build.ps1 -Clean

# Show help
.\build.ps1 -Help
```

### Option 3: Makefile (MinGW Make)

```bash
# Show help
make

# Build specific example
make build BACKEND=sdl EXAMPLE=sample1
make build BACKEND=win32 EXAMPLE=sample2

# Build all examples
make build-all BACKEND=sdl

# Build and run
make run BACKEND=sdl EXAMPLE=sample1

# Clean
make clean
```

### Prerequisites for Windows

1. **MinGW** with g++ compiler
2. **SDL2** (only for SDL backend):
   - Download from [libsdl.org](https://www.libsdl.org/download-2.0.php)
   - Get `SDL2-devel-2.x.x-mingw.tar.gz`
   - Extract to `C:\SDL2` or set `SDL2_PATH` environment variable

**Note**: Win32 backend has no external dependencies!

## Building on Linux

### Using Makefile

```bash
# Build with SDL2
make build BACKEND=sdl EXAMPLE=sample1

# Build with X11 (native Linux)
make build BACKEND=x11 EXAMPLE=sample1

# Build all examples
make build-all BACKEND=x11

# Clean
make clean
```

### Prerequisites for Linux

For SDL2 backend:
```bash
sudo apt-get install libsdl2-dev  # Debian/Ubuntu
sudo dnf install SDL2-devel       # Fedora
```

For X11 backend:
```bash
sudo apt-get install libx11-dev   # Debian/Ubuntu
sudo dnf install libX11-devel     # Fedora
```

## Manual Compilation

### Windows with SDL2
```cmd
g++ -std=c++11 -DUSE_SDL -o build\sample1.exe ^
    examples\sample1.cpp lib\graphics.cpp ^
    -IC:\SDL2\include ^
    -LC:\SDL2\lib ^
    -lmingw32 -lSDL2main -lSDL2 -mwindows
```

### Windows with Win32
```cmd
g++ -std=c++11 -DUSE_WIN32 -o build\sample1.exe ^
    examples\sample1.cpp lib\graphics.cpp ^
    -lgdi32 -luser32
```

### Linux with SDL2
```bash
g++ -std=c++11 -DUSE_SDL -o build/sample1 \
    examples/sample1.cpp lib/graphics.cpp \
    $(sdl2-config --cflags --libs)
```

### Linux with X11
```bash
g++ -std=c++11 -DUSE_X11 -o build/sample1 \
    examples/sample1.cpp lib/graphics.cpp \
    -lX11
```

## Usage Example

```cpp
#include "graphics.h"

int main() {
    // Create a window
    WindowHandle* window = createWindow("My Window", 800, 600);
    
    // Main loop
    while (!windowShouldClose(window)) {
        pollEvents(window);
        
        // Clear screen
        clearScreen(window, Color(0, 0, 0));
        
        // Draw shapes
        drawLine(window, 10, 10, 100, 100, Color(255, 0, 0));
        drawFilledCircle(window, 200, 200, 50, Color(0, 255, 0));
        drawRectangle(window, 300, 300, 100, 100, Color(0, 0, 255));
        
        // Present
        swapBuffers(window);
        
        delay(16); // ~60 FPS
    }
    
    // Cleanup
    destroyWindow(window);
    return 0;
}
```

## API Reference

### Window Management
- `WindowHandle* createWindow(const char* title, int width, int height)` - Create a new window
- `void destroyWindow(WindowHandle* window)` - Destroy window and free resources
- `bool windowShouldClose(WindowHandle* window)` - Check if window should close
- `void pollEvents(WindowHandle* window)` - Process window events
- `void swapBuffers(WindowHandle* window)` - Present rendered frame

### Drawing Functions
- `void clearScreen(WindowHandle* window, const Color& color)` - Clear screen with color
- `void drawLine(WindowHandle* window, int x1, int y1, int x2, int y2, const Color& color)` - Draw a line
- `void drawRectangle(WindowHandle* window, int x, int y, int w, int h, const Color& color)` - Draw rectangle outline
- `void drawFilledRectangle(WindowHandle* window, int x, int y, int w, int h, const Color& color)` - Draw filled rectangle
- `void drawCircle(WindowHandle* window, int cx, int cy, int radius, const Color& color)` - Draw circle outline
- `void drawFilledCircle(WindowHandle* window, int cx, int cy, int radius, const Color& color)` - Draw filled circle
- `void drawPixel(WindowHandle* window, int x, int y, const Color& color)` - Draw a single pixel

### Utility Functions
- `void setDrawColor(WindowHandle* window, const Color& color)` - Set current drawing color
- `void delay(uint32_t milliseconds)` - Delay execution

### Color Structure
```cpp
struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
};
```

## Backend Comparison

| Feature | SDL2 | Win32 | X11 |
|---------|------|-------|-----|
| Platform | Cross-platform | Windows only | Linux only |
| Dependencies | SDL2 library | None (native) | X11 library |
| Performance | Good | Excellent | Good |
| Complexity | Easy | Medium | Medium |
| Alpha blending | Yes | Limited | Limited |

## Project Structure

```
libgraffik/
├── lib/
│   ├── graphics.h       # Header file with API declarations
│   ├── graphics.cpp     # Implementation for all backends
│   └── SDL2.dll         # SDL2 DLL (Windows only)
├── examples/
│   ├── sample1.cpp      # Basic shapes demo
│   ├── sample2.cpp      # Animation demo
│   └── sample3.cpp      # Interactive demo
├── build/               # Output directory (created automatically)
├── Makefile             # Unix-style Makefile
├── build.ps1            # PowerShell build script
├── build.bat            # Batch build script
└── README.md            # This file
```

## Native options

- **Windows native**: Use `build.bat` for CMD or `build.ps1` for PowerShell.
- **Linux users**: Use the universal Makefile, also most recommended for all OSes.
- **Win32 backend**: Windows-native, uses CPU for rendering, may cause lack of performance, although may use less resources.
- **SDL2 backend**: Best for cross-platform development, works natively on both Linux and Windows, uses GPU for rendering. May be more resource heavy than these simplier backends like X11 or Win32.
- **X11 backend**: Native Linux performance, uses CPU for rendering. Will work for desktops using Xorg server.

## License

MIT License. See [LICENSE](LICENSE) file for details.

# Important notice

AI had a hand in this. I try to review and improve everything it generates.
