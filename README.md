# Graffiklib

A simple cross-platform graphics abstraction layer with support for multiple backends (SDL2, Win32, X11).

## Current Implementation

Currently implemented backends:
- **SDL2** (Windows) - Fully implemented

Planned backends:
- **Win32** (Windows native) - Placeholder
- **X11** (Linux) - Placeholder

## Features

- Window creation and management
- Basic drawing primitives:
  - Lines
  - Rectangles (filled and outlined)
  - Circles (filled and outlined)
  - Pixels
- Color support with alpha channel
- Simple event handling
- Cross-platform delay function

## Building

### Windows (with SDL2)

#### Prerequisites
1. **C++ compiler**: MinGW or MSVC
2. **SDL2 development libraries**: Download from [libsdl.org](https://www.libsdl.org/download-2.0.php)
   - For MinGW: Download `SDL2-devel-2.x.x-mingw.tar.gz`
   - For MSVC: Download `SDL2-devel-2.x.x-VC.zip`

#### Installing SDL2

1. **Download SDL2** development libraries for your compiler
2. **Extract** the archive
3. **Place SDL2** in one of these locations:
   - `C:/SDL2`
   - `D:/SDL2`
   - Or in your project directory: `<project>/SDL2`

#### Option 1: Using Makefile (MinGW - Easiest)

```bash
# If SDL2 is in C:/SDL2
make

# If SDL2 is elsewhere
make SDL2_PATH=D:/path/to/SDL2

# Run the demo
make run
```

**Note**: Make sure `SDL2.dll` from `SDL2/bin` is in the same directory as the executable or in your PATH.

#### Option 2: Using CMake

```bash
mkdir build
cd build

# If SDL2 is in a standard location (C:/SDL2 or D:/SDL2)
cmake ..

# If SDL2 is elsewhere
cmake .. -DCMAKE_PREFIX_PATH=D:/path/to/SDL2

# Build
cmake --build .
```

The CMakeLists.txt will automatically copy SDL2.dll to the build directory.

#### Option 3: Manual compilation with g++/MinGW

```bash
# Adjust paths to match your SDL2 installation
g++ -std=c++11 -DUSE_SDL -o GraphicsDemo.exe main.cpp graphics.cpp ^
    -IC:/SDL2/include ^
    -LC:/SDL2/lib ^
    -lmingw32 -lSDL2main -lSDL2 -mwindows

# Copy SDL2.dll to the same directory
copy C:\SDL2\bin\SDL2.dll .
```

#### Option 4: Manual compilation with MSVC

```bash
cl /EHsc /DUSE_SDL main.cpp graphics.cpp ^
   /I"C:\SDL2\include" ^
   /link "C:\SDL2\lib\x64\SDL2.lib" "C:\SDL2\lib\x64\SDL2main.lib" ^
   /SUBSYSTEM:CONSOLE

# Copy SDL2.dll
copy C:\SDL2\lib\x64\SDL2.dll .
```

### Linux (X11 - Not yet implemented)
```bash
g++ -o demo main.cpp graphics.cpp -lX11 -DUSE_X11
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

## Architecture

The library uses a simple abstraction pattern:

1. **graphics.h** - Platform-independent API declarations
2. **graphics.cpp** - Platform-specific implementations (selected at compile time)
3. **Compile-time selection** - Backend is chosen via preprocessor defines

### Adding a New Backend

To add a new backend (e.g., Vulkan, DirectX):

1. Add a new `#ifdef USE_BACKEND` section in `graphics.cpp`
2. Implement all functions declared in `graphics.h`
3. Update the platform detection logic in `graphics.h` if needed
4. Add compilation instructions to this README

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

## License

MIT License. See [LICENSE](LICENSE) file.