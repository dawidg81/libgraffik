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

// ============================================================================
// INPUT HANDLING
// ============================================================================

// Key codes (cross-platform)
enum KeyCode {
    KEY_UNKNOWN = 0,
    
    // Arrow keys
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    
    // Letter keys
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
    KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P,
    KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
    KEY_Y, KEY_Z,
    
    // Number keys
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    // Function keys
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    
    // Special keys
    KEY_SPACE,
    KEY_ENTER,
    KEY_ESCAPE,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_SHIFT,
    KEY_CONTROL,
    KEY_ALT,
    
    // Other
    KEY_PLUS,
    KEY_MINUS,
    
    KEY_COUNT // Total number of keys
};

// Mouse buttons
enum MouseButton {
    MOUSE_LEFT = 0,
    MOUSE_RIGHT,
    MOUSE_MIDDLE,
    MOUSE_BUTTON_COUNT
};

// Keyboard input functions
bool keyDown(WindowHandle* window, KeyCode key);
bool keyPressed(WindowHandle* window, KeyCode key);  // True only on the frame the key was pressed
bool keyReleased(WindowHandle* window, KeyCode key); // True only on the frame the key was released

// Mouse input functions
bool mouseDown(WindowHandle* window, MouseButton button);
bool mousePressed(WindowHandle* window, MouseButton button);
bool mouseReleased(WindowHandle* window, MouseButton button);

void getMousePosition(WindowHandle* window, int& x, int& y);
void getMouseDelta(WindowHandle* window, int& dx, int& dy);
void setMousePosition(WindowHandle* window, int x, int y);
void setMouseLocked(WindowHandle* window, bool locked); // Locks mouse to window center (for FPS games)
bool isMouseLocked(WindowHandle* window);

// Mouse wheel
int getMouseWheelDelta(WindowHandle* window);

#endif // GRAPHICS_H