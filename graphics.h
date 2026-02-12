#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__)
    #define PLATFORM_MACOS
#endif

// Backend selection
// Define one of these before including this header, or let it auto-detect
#if !defined(USE_SDL) && !defined(USE_WIN32) && !defined(USE_X11)
    #ifdef PLATFORM_WINDOWS
        #define USE_SDL  // Default to SDL on Windows for now
    #elif defined(PLATFORM_LINUX)
        #define USE_X11
    #endif
#endif

// Color structure
struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};

// Forward declarations for platform-specific types
struct WindowHandle;

// Window management functions
WindowHandle* createWindow(const char* title, int width, int height);
void destroyWindow(WindowHandle* window);
bool windowShouldClose(WindowHandle* window);
void pollEvents(WindowHandle* window);
void swapBuffers(WindowHandle* window);

// Drawing functions
void clearScreen(WindowHandle* window, const Color& color);
void drawLine(WindowHandle* window, int x1, int y1, int x2, int y2, const Color& color);
void drawRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color);
void drawFilledRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color);
void drawCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color);
void drawFilledCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color);
void drawPixel(WindowHandle* window, int x, int y, const Color& color);

// Utility functions
void setDrawColor(WindowHandle* window, const Color& color);
void delay(uint32_t milliseconds);

#endif // GRAPHICS_H