#include "graphics.h"
#include <cmath>

#ifdef USE_SDL

#include <SDL2/SDL.h>
#include <cstring>

// Platform-specific window handle structure
struct WindowHandle {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool shouldClose;
    
    // Input state
    bool keyState[KEY_COUNT];
    bool prevKeyState[KEY_COUNT];
    
    bool mouseState[MOUSE_BUTTON_COUNT];
    bool prevMouseState[MOUSE_BUTTON_COUNT];
    
    int mouseX, mouseY;
    int prevMouseX, prevMouseY;
    int mouseDeltaX, mouseDeltaY;
    
    int mouseWheelDelta;
    
    bool mouseLocked;
    
    WindowHandle() : window(nullptr), renderer(nullptr), shouldClose(false),
                     mouseX(0), mouseY(0), prevMouseX(0), prevMouseY(0),
                     mouseDeltaX(0), mouseDeltaY(0), mouseWheelDelta(0),
                     mouseLocked(false) {
        memset(keyState, 0, sizeof(keyState));
        memset(prevKeyState, 0, sizeof(prevKeyState));
        memset(mouseState, 0, sizeof(mouseState));
        memset(prevMouseState, 0, sizeof(prevMouseState));
    }
};

// Initialize SDL (called once)
static bool sdlInitialized = false;

static void ensureSDLInit() {
    if (!sdlInitialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            // Handle error - in production, use proper error handling
            return;
        }
        sdlInitialized = true;
    }
}

// SDL key mapping
static KeyCode mapSDLKey(SDL_Keycode sdlKey) {
    switch (sdlKey) {
        case SDLK_UP: return KEY_UP;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_LEFT: return KEY_LEFT;
        case SDLK_RIGHT: return KEY_RIGHT;
        
        case SDLK_a: return KEY_A;
        case SDLK_b: return KEY_B;
        case SDLK_c: return KEY_C;
        case SDLK_d: return KEY_D;
        case SDLK_e: return KEY_E;
        case SDLK_f: return KEY_F;
        case SDLK_g: return KEY_G;
        case SDLK_h: return KEY_H;
        case SDLK_i: return KEY_I;
        case SDLK_j: return KEY_J;
        case SDLK_k: return KEY_K;
        case SDLK_l: return KEY_L;
        case SDLK_m: return KEY_M;
        case SDLK_n: return KEY_N;
        case SDLK_o: return KEY_O;
        case SDLK_p: return KEY_P;
        case SDLK_q: return KEY_Q;
        case SDLK_r: return KEY_R;
        case SDLK_s: return KEY_S;
        case SDLK_t: return KEY_T;
        case SDLK_u: return KEY_U;
        case SDLK_v: return KEY_V;
        case SDLK_w: return KEY_W;
        case SDLK_x: return KEY_X;
        case SDLK_y: return KEY_Y;
        case SDLK_z: return KEY_Z;
        
        case SDLK_0: return KEY_0;
        case SDLK_1: return KEY_1;
        case SDLK_2: return KEY_2;
        case SDLK_3: return KEY_3;
        case SDLK_4: return KEY_4;
        case SDLK_5: return KEY_5;
        case SDLK_6: return KEY_6;
        case SDLK_7: return KEY_7;
        case SDLK_8: return KEY_8;
        case SDLK_9: return KEY_9;
        
        case SDLK_F1: return KEY_F1;
        case SDLK_F2: return KEY_F2;
        case SDLK_F3: return KEY_F3;
        case SDLK_F4: return KEY_F4;
        case SDLK_F5: return KEY_F5;
        case SDLK_F6: return KEY_F6;
        case SDLK_F7: return KEY_F7;
        case SDLK_F8: return KEY_F8;
        case SDLK_F9: return KEY_F9;
        case SDLK_F10: return KEY_F10;
        case SDLK_F11: return KEY_F11;
        case SDLK_F12: return KEY_F12;
        
        case SDLK_SPACE: return KEY_SPACE;
        case SDLK_RETURN: return KEY_ENTER;
        case SDLK_ESCAPE: return KEY_ESCAPE;
        case SDLK_BACKSPACE: return KEY_BACKSPACE;
        case SDLK_TAB: return KEY_TAB;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT: return KEY_SHIFT;
        case SDLK_LCTRL:
        case SDLK_RCTRL: return KEY_CONTROL;
        case SDLK_LALT:
        case SDLK_RALT: return KEY_ALT;
        
        case SDLK_PLUS:
        case SDLK_EQUALS: return KEY_PLUS;
        case SDLK_MINUS: return KEY_MINUS;
        
        default: return KEY_UNKNOWN;
    }
}

WindowHandle* createWindow(const char* title, int width, int height) {
    ensureSDLInit();
    
    WindowHandle* handle = new WindowHandle();
    
    handle->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );
    
    if (!handle->window) {
        delete handle;
        return nullptr;
    }
    
    handle->renderer = SDL_CreateRenderer(
        handle->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!handle->renderer) {
        SDL_DestroyWindow(handle->window);
        delete handle;
        return nullptr;
    }
    
    // Enable alpha blending
    SDL_SetRenderDrawBlendMode(handle->renderer, SDL_BLENDMODE_BLEND);
    
    return handle;
}

void destroyWindow(WindowHandle* window) {
    if (!window) return;
    
    if (window->renderer) {
        SDL_DestroyRenderer(window->renderer);
    }
    
    if (window->window) {
        SDL_DestroyWindow(window->window);
    }
    
    delete window;
    
    // Cleanup SDL if this was the last window
    if (sdlInitialized) {
        SDL_Quit();
        sdlInitialized = false;
    }
}

bool windowShouldClose(WindowHandle* window) {
    if (!window) return true;
    return window->shouldClose;
}

void pollEvents(WindowHandle* window) {
    if (!window) return;
    
    // Save previous state
    memcpy(window->prevKeyState, window->keyState, sizeof(window->keyState));
    memcpy(window->prevMouseState, window->mouseState, sizeof(window->mouseState));
    
    window->prevMouseX = window->mouseX;
    window->prevMouseY = window->mouseY;
    window->mouseWheelDelta = 0;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            window->shouldClose = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            KeyCode key = mapSDLKey(event.key.keysym.sym);
            if (key != KEY_UNKNOWN && key < KEY_COUNT) {
                window->keyState[key] = true;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                window->shouldClose = true;
            }
        }
        else if (event.type == SDL_KEYUP) {
            KeyCode key = mapSDLKey(event.key.keysym.sym);
            if (key != KEY_UNKNOWN && key < KEY_COUNT) {
                window->keyState[key] = false;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT)
                window->mouseState[MOUSE_LEFT] = true;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                window->mouseState[MOUSE_RIGHT] = true;
            else if (event.button.button == SDL_BUTTON_MIDDLE)
                window->mouseState[MOUSE_MIDDLE] = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT)
                window->mouseState[MOUSE_LEFT] = false;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                window->mouseState[MOUSE_RIGHT] = false;
            else if (event.button.button == SDL_BUTTON_MIDDLE)
                window->mouseState[MOUSE_MIDDLE] = false;
        }
        else if (event.type == SDL_MOUSEMOTION) {
            window->mouseX = event.motion.x;
            window->mouseY = event.motion.y;
        }
        else if (event.type == SDL_MOUSEWHEEL) {
            window->mouseWheelDelta = event.wheel.y;
        }
    }
    
    // Calculate mouse delta
    window->mouseDeltaX = window->mouseX - window->prevMouseX;
    window->mouseDeltaY = window->mouseY - window->prevMouseY;
    
    // Handle mouse locking
    if (window->mouseLocked) {
        int centerX, centerY;
        SDL_GetWindowSize(window->window, &centerX, &centerY);
        centerX /= 2;
        centerY /= 2;
        
        SDL_WarpMouseInWindow(window->window, centerX, centerY);
        window->mouseX = centerX;
        window->mouseY = centerY;
    }
}

void swapBuffers(WindowHandle* window) {
    if (!window || !window->renderer) return;
    SDL_RenderPresent(window->renderer);
}

void clearScreen(WindowHandle* window, const Color& color) {
    if (!window || !window->renderer) return;
    
    SDL_SetRenderDrawColor(window->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(window->renderer);
}

void setDrawColor(WindowHandle* window, const Color& color) {
    if (!window || !window->renderer) return;
    SDL_SetRenderDrawColor(window->renderer, color.r, color.g, color.b, color.a);
}

void drawLine(WindowHandle* window, int x1, int y1, int x2, int y2, const Color& color) {
    if (!window || !window->renderer) return;
    
    setDrawColor(window, color);
    SDL_RenderDrawLine(window->renderer, x1, y1, x2, y2);
}

void drawRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color) {
    if (!window || !window->renderer) return;
    
    SDL_Rect rect = {x, y, width, height};
    setDrawColor(window, color);
    SDL_RenderDrawRect(window->renderer, &rect);
}

void drawFilledRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color) {
    if (!window || !window->renderer) return;
    
    SDL_Rect rect = {x, y, width, height};
    setDrawColor(window, color);
    SDL_RenderFillRect(window->renderer, &rect);
}

void drawPixel(WindowHandle* window, int x, int y, const Color& color) {
    if (!window || !window->renderer) return;
    
    setDrawColor(window, color);
    SDL_RenderDrawPoint(window->renderer, x, y);
}

// Helper function for drawing circles using midpoint circle algorithm
void drawCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color) {
    if (!window || !window->renderer) return;
    
    setDrawColor(window, color);
    
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    auto drawCirclePoints = [&](int xc, int yc, int x, int y) {
        SDL_RenderDrawPoint(window->renderer, xc + x, yc + y);
        SDL_RenderDrawPoint(window->renderer, xc - x, yc + y);
        SDL_RenderDrawPoint(window->renderer, xc + x, yc - y);
        SDL_RenderDrawPoint(window->renderer, xc - x, yc - y);
        SDL_RenderDrawPoint(window->renderer, xc + y, yc + x);
        SDL_RenderDrawPoint(window->renderer, xc - y, yc + x);
        SDL_RenderDrawPoint(window->renderer, xc + y, yc - x);
        SDL_RenderDrawPoint(window->renderer, xc - y, yc - x);
    };
    
    drawCirclePoints(centerX, centerY, x, y);
    
    while (y >= x) {
        x++;
        
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        
        drawCirclePoints(centerX, centerY, x, y);
    }
}

void drawFilledCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color) {
    if (!window || !window->renderer) return;
    
    setDrawColor(window, color);
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(window->renderer, centerX + x, centerY + y);
            }
        }
    }
}

void delay(uint32_t milliseconds) {
    SDL_Delay(milliseconds);
}

// ============================================================================
// INPUT HANDLING - SDL
// ============================================================================

bool keyDown(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return window->keyState[key];
}

bool keyPressed(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return window->keyState[key] && !window->prevKeyState[key];
}

bool keyReleased(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return !window->keyState[key] && window->prevKeyState[key];
}

bool mouseDown(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return window->mouseState[button];
}

bool mousePressed(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return window->mouseState[button] && !window->prevMouseState[button];
}

bool mouseReleased(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return !window->mouseState[button] && window->prevMouseState[button];
}

void getMousePosition(WindowHandle* window, int& x, int& y) {
    if (!window) {
        x = y = 0;
        return;
    }
    x = window->mouseX;
    y = window->mouseY;
}

void getMouseDelta(WindowHandle* window, int& dx, int& dy) {
    if (!window) {
        dx = dy = 0;
        return;
    }
    dx = window->mouseDeltaX;
    dy = window->mouseDeltaY;
}

void setMousePosition(WindowHandle* window, int x, int y) {
    if (!window || !window->window) return;
    SDL_WarpMouseInWindow(window->window, x, y);
    window->mouseX = x;
    window->mouseY = y;
}

void setMouseLocked(WindowHandle* window, bool locked) {
    if (!window || !window->window) return;
    window->mouseLocked = locked;
    
    if (locked) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
    }
}

bool isMouseLocked(WindowHandle* window) {
    if (!window) return false;
    return window->mouseLocked;
}

int getMouseWheelDelta(WindowHandle* window) {
    if (!window) return 0;
    return window->mouseWheelDelta;
}

#endif // USE_SDL

#ifdef USE_WIN32

#include <windows.h>
#include <vector>
#include <cstring>

// Platform-specific window handle structure for Win32
struct WindowHandle {
    HWND hwnd;
    HDC hdc;
    HDC memDC;
    HBITMAP memBitmap;
    HBITMAP oldBitmap;
    int width;
    int height;
    bool shouldClose;
    COLORREF currentColor;
    
    // Input state
    bool keyState[KEY_COUNT];
    bool prevKeyState[KEY_COUNT];
    
    bool mouseState[MOUSE_BUTTON_COUNT];
    bool prevMouseState[MOUSE_BUTTON_COUNT];
    
    int mouseX, mouseY;
    int prevMouseX, prevMouseY;
    int mouseDeltaX, mouseDeltaY;
    
    int mouseWheelDelta;
    
    bool mouseLocked;
    
    WindowHandle() : hwnd(nullptr), hdc(nullptr), memDC(nullptr), 
                     memBitmap(nullptr), oldBitmap(nullptr),
                     width(0), height(0), shouldClose(false), 
                     currentColor(RGB(255, 255, 255)),
                     mouseX(0), mouseY(0), prevMouseX(0), prevMouseY(0),
                     mouseDeltaX(0), mouseDeltaY(0), mouseWheelDelta(0),
                     mouseLocked(false) {
        memset(keyState, 0, sizeof(keyState));
        memset(prevKeyState, 0, sizeof(prevKeyState));
        memset(mouseState, 0, sizeof(mouseState));
        memset(prevMouseState, 0, sizeof(prevMouseState));
    }
};

// Global window class name
static const char* WINDOW_CLASS_NAME = "LibGraffikWindowClass";
static bool classRegistered = false;

// Win32 key mapping
static KeyCode mapWin32Key(WPARAM wParam) {
    switch (wParam) {
        case VK_UP: return KEY_UP;
        case VK_DOWN: return KEY_DOWN;
        case VK_LEFT: return KEY_LEFT;
        case VK_RIGHT: return KEY_RIGHT;
        
        case 'A': return KEY_A;
        case 'B': return KEY_B;
        case 'C': return KEY_C;
        case 'D': return KEY_D;
        case 'E': return KEY_E;
        case 'F': return KEY_F;
        case 'G': return KEY_G;
        case 'H': return KEY_H;
        case 'I': return KEY_I;
        case 'J': return KEY_J;
        case 'K': return KEY_K;
        case 'L': return KEY_L;
        case 'M': return KEY_M;
        case 'N': return KEY_N;
        case 'O': return KEY_O;
        case 'P': return KEY_P;
        case 'Q': return KEY_Q;
        case 'R': return KEY_R;
        case 'S': return KEY_S;
        case 'T': return KEY_T;
        case 'U': return KEY_U;
        case 'V': return KEY_V;
        case 'W': return KEY_W;
        case 'X': return KEY_X;
        case 'Y': return KEY_Y;
        case 'Z': return KEY_Z;
        
        case '0': return KEY_0;
        case '1': return KEY_1;
        case '2': return KEY_2;
        case '3': return KEY_3;
        case '4': return KEY_4;
        case '5': return KEY_5;
        case '6': return KEY_6;
        case '7': return KEY_7;
        case '8': return KEY_8;
        case '9': return KEY_9;
        
        case VK_F1: return KEY_F1;
        case VK_F2: return KEY_F2;
        case VK_F3: return KEY_F3;
        case VK_F4: return KEY_F4;
        case VK_F5: return KEY_F5;
        case VK_F6: return KEY_F6;
        case VK_F7: return KEY_F7;
        case VK_F8: return KEY_F8;
        case VK_F9: return KEY_F9;
        case VK_F10: return KEY_F10;
        case VK_F11: return KEY_F11;
        case VK_F12: return KEY_F12;
        
        case VK_SPACE: return KEY_SPACE;
        case VK_RETURN: return KEY_ENTER;
        case VK_ESCAPE: return KEY_ESCAPE;
        case VK_BACK: return KEY_BACKSPACE;
        case VK_TAB: return KEY_TAB;
        case VK_SHIFT: return KEY_SHIFT;
        case VK_CONTROL: return KEY_CONTROL;
        case VK_MENU: return KEY_ALT;
        
        case VK_OEM_PLUS: return KEY_PLUS;
        case VK_OEM_MINUS: return KEY_MINUS;
        
        default: return KEY_UNKNOWN;
    }
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowHandle* handle = (WindowHandle*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (uMsg) {
        case WM_CLOSE:
            if (handle) {
                handle->shouldClose = true;
            }
            return 0;
            
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            if (handle) {
                KeyCode key = mapWin32Key(wParam);
                if (key != KEY_UNKNOWN && key < KEY_COUNT) {
                    handle->keyState[key] = true;
                }
                if (wParam == VK_ESCAPE) {
                    handle->shouldClose = true;
                }
            }
            return 0;
        }
            
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            if (handle) {
                KeyCode key = mapWin32Key(wParam);
                if (key != KEY_UNKNOWN && key < KEY_COUNT) {
                    handle->keyState[key] = false;
                }
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN:
            if (handle) handle->mouseState[MOUSE_LEFT] = true;
            return 0;
            
        case WM_LBUTTONUP:
            if (handle) handle->mouseState[MOUSE_LEFT] = false;
            return 0;
            
        case WM_RBUTTONDOWN:
            if (handle) handle->mouseState[MOUSE_RIGHT] = true;
            return 0;
            
        case WM_RBUTTONUP:
            if (handle) handle->mouseState[MOUSE_RIGHT] = false;
            return 0;
            
        case WM_MBUTTONDOWN:
            if (handle) handle->mouseState[MOUSE_MIDDLE] = true;
            return 0;
            
        case WM_MBUTTONUP:
            if (handle) handle->mouseState[MOUSE_MIDDLE] = false;
            return 0;
            
        case WM_MOUSEMOVE:
            if (handle) {
                handle->mouseX = LOWORD(lParam);
                handle->mouseY = HIWORD(lParam);
            }
            return 0;
            
        case WM_MOUSEWHEEL:
            if (handle) {
                handle->mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            }
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // Will be handled by swapBuffers
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowHandle* createWindow(const char* title, int width, int height) {
    // Register window class if not already registered
    if (!classRegistered) {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = WINDOW_CLASS_NAME;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        
        if (!RegisterClassA(&wc)) {
            return nullptr;
        }
        classRegistered = true;
    }
    
    WindowHandle* handle = new WindowHandle();
    handle->width = width;
    handle->height = height;
    
    // Create window
    handle->hwnd = CreateWindowExA(
        0,
        WINDOW_CLASS_NAME,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!handle->hwnd) {
        delete handle;
        return nullptr;
    }
    
    // Store handle in window user data
    SetWindowLongPtr(handle->hwnd, GWLP_USERDATA, (LONG_PTR)handle);
    
    // Get device context
    handle->hdc = GetDC(handle->hwnd);
    
    // Create memory DC for double buffering
    handle->memDC = CreateCompatibleDC(handle->hdc);
    handle->memBitmap = CreateCompatibleBitmap(handle->hdc, width, height);
    handle->oldBitmap = (HBITMAP)SelectObject(handle->memDC, handle->memBitmap);
    
    // Show window
    ShowWindow(handle->hwnd, SW_SHOW);
    UpdateWindow(handle->hwnd);
    
    return handle;
}

void destroyWindow(WindowHandle* window) {
    if (!window) return;
    
    if (window->memDC) {
        if (window->oldBitmap) {
            SelectObject(window->memDC, window->oldBitmap);
        }
        DeleteDC(window->memDC);
    }
    
    if (window->memBitmap) {
        DeleteObject(window->memBitmap);
    }
    
    if (window->hdc) {
        ReleaseDC(window->hwnd, window->hdc);
    }
    
    if (window->hwnd) {
        DestroyWindow(window->hwnd);
    }
    
    delete window;
}

bool windowShouldClose(WindowHandle* window) {
    if (!window) return true;
    return window->shouldClose;
}

void pollEvents(WindowHandle* window) {
    if (!window) return;
    
    // Save previous state
    memcpy(window->prevKeyState, window->keyState, sizeof(window->keyState));
    memcpy(window->prevMouseState, window->mouseState, sizeof(window->mouseState));
    
    window->prevMouseX = window->mouseX;
    window->prevMouseY = window->mouseY;
    window->mouseWheelDelta = 0;
    
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Calculate mouse delta
    window->mouseDeltaX = window->mouseX - window->prevMouseX;
    window->mouseDeltaY = window->mouseY - window->prevMouseY;
    
    // Handle mouse locking
    if (window->mouseLocked && window->hwnd) {
        RECT rect;
        GetClientRect(window->hwnd, &rect);
        int centerX = rect.right / 2;
        int centerY = rect.bottom / 2;
        
        POINT pt = {centerX, centerY};
        ClientToScreen(window->hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
        
        window->mouseX = centerX;
        window->mouseY = centerY;
    }
}

void swapBuffers(WindowHandle* window) {
    if (!window || !window->hdc || !window->memDC) return;
    
    // Copy from memory DC to window DC
    BitBlt(window->hdc, 0, 0, window->width, window->height,
           window->memDC, 0, 0, SRCCOPY);
}

void clearScreen(WindowHandle* window, const Color& color) {
    if (!window || !window->memDC) return;
    
    RECT rect = {0, 0, window->width, window->height};
    HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
    FillRect(window->memDC, &rect, brush);
    DeleteObject(brush);
}

void setDrawColor(WindowHandle* window, const Color& color) {
    if (!window) return;
    window->currentColor = RGB(color.r, color.g, color.b);
}

void drawLine(WindowHandle* window, int x1, int y1, int x2, int y2, const Color& color) {
    if (!window || !window->memDC) return;
    
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(color.r, color.g, color.b));
    HPEN oldPen = (HPEN)SelectObject(window->memDC, pen);
    
    MoveToEx(window->memDC, x1, y1, nullptr);
    LineTo(window->memDC, x2, y2);
    
    SelectObject(window->memDC, oldPen);
    DeleteObject(pen);
}

void drawRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color) {
    if (!window || !window->memDC) return;
    
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(color.r, color.g, color.b));
    HPEN oldPen = (HPEN)SelectObject(window->memDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(window->memDC, GetStockObject(NULL_BRUSH));
    
    Rectangle(window->memDC, x, y, x + width, y + height);
    
    SelectObject(window->memDC, oldPen);
    SelectObject(window->memDC, oldBrush);
    DeleteObject(pen);
}

void drawFilledRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color) {
    if (!window || !window->memDC) return;
    
    RECT rect = {x, y, x + width, y + height};
    HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
    FillRect(window->memDC, &rect, brush);
    DeleteObject(brush);
}

void drawPixel(WindowHandle* window, int x, int y, const Color& color) {
    if (!window || !window->memDC) return;
    
    SetPixel(window->memDC, x, y, RGB(color.r, color.g, color.b));
}

// Helper function for drawing circles using midpoint circle algorithm
static void drawCirclePoints(HDC dc, int xc, int yc, int x, int y, COLORREF color) {
    SetPixel(dc, xc + x, yc + y, color);
    SetPixel(dc, xc - x, yc + y, color);
    SetPixel(dc, xc + x, yc - y, color);
    SetPixel(dc, xc - x, yc - y, color);
    SetPixel(dc, xc + y, yc + x, color);
    SetPixel(dc, xc - y, yc + x, color);
    SetPixel(dc, xc + y, yc - x, color);
    SetPixel(dc, xc - y, yc - x, color);
}

void drawCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color) {
    if (!window || !window->memDC) return;
    
    COLORREF col = RGB(color.r, color.g, color.b);
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    drawCirclePoints(window->memDC, centerX, centerY, x, y, col);
    
    while (y >= x) {
        x++;
        
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        
        drawCirclePoints(window->memDC, centerX, centerY, x, y, col);
    }
}

void drawFilledCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color) {
    if (!window || !window->memDC) return;
    
    COLORREF col = RGB(color.r, color.g, color.b);
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SetPixel(window->memDC, centerX + x, centerY + y, col);
            }
        }
    }
}

void delay(uint32_t milliseconds) {
    Sleep(milliseconds);
}

// ============================================================================
// INPUT HANDLING - WIN32
// ============================================================================

bool keyDown(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return window->keyState[key];
}

bool keyPressed(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return window->keyState[key] && !window->prevKeyState[key];
}

bool keyReleased(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return !window->keyState[key] && window->prevKeyState[key];
}

bool mouseDown(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return window->mouseState[button];
}

bool mousePressed(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return window->mouseState[button] && !window->prevMouseState[button];
}

bool mouseReleased(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return !window->mouseState[button] && window->prevMouseState[button];
}

void getMousePosition(WindowHandle* window, int& x, int& y) {
    if (!window) {
        x = y = 0;
        return;
    }
    x = window->mouseX;
    y = window->mouseY;
}

void getMouseDelta(WindowHandle* window, int& dx, int& dy) {
    if (!window) {
        dx = dy = 0;
        return;
    }
    dx = window->mouseDeltaX;
    dy = window->mouseDeltaY;
}

void setMousePosition(WindowHandle* window, int x, int y) {
    if (!window || !window->hwnd) return;
    
    POINT pt = {x, y};
    ClientToScreen(window->hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
    
    window->mouseX = x;
    window->mouseY = y;
}

void setMouseLocked(WindowHandle* window, bool locked) {
    if (!window) return;
    window->mouseLocked = locked;
    
    if (locked) {
        ShowCursor(FALSE);
    } else {
        ShowCursor(TRUE);
    }
}

bool isMouseLocked(WindowHandle* window) {
    if (!window) return false;
    return window->mouseLocked;
}

int getMouseWheelDelta(WindowHandle* window) {
    if (!window) return 0;
    return window->mouseWheelDelta;
}

#endif // USE_WIN32

#ifdef USE_X11

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <cstring>

// Platform-specific window handle structure for X11
struct WindowHandle {
    Display* display;
    Window window;
    GC gc;
    Pixmap backBuffer;
    int width;
    int height;
    bool shouldClose;
    Atom wmDeleteMessage;
    unsigned long currentColor;
    
    // Input state
    bool keyState[KEY_COUNT];
    bool prevKeyState[KEY_COUNT];
    
    bool mouseState[MOUSE_BUTTON_COUNT];
    bool prevMouseState[MOUSE_BUTTON_COUNT];
    
    int mouseX, mouseY;
    int prevMouseX, prevMouseY;
    int mouseDeltaX, mouseDeltaY;
    
    int mouseWheelDelta;
    
    bool mouseLocked;
    
    WindowHandle() : display(nullptr), window(0), gc(nullptr), 
                     backBuffer(0), width(0), height(0), 
                     shouldClose(false), wmDeleteMessage(0),
                     currentColor(0xFFFFFF),
                     mouseX(0), mouseY(0), prevMouseX(0), prevMouseY(0),
                     mouseDeltaX(0), mouseDeltaY(0), mouseWheelDelta(0),
                     mouseLocked(false) {
        memset(keyState, 0, sizeof(keyState));
        memset(prevKeyState, 0, sizeof(prevKeyState));
        memset(mouseState, 0, sizeof(mouseState));
        memset(prevMouseState, 0, sizeof(prevMouseState));
    }
};

// Helper to convert Color to X11 pixel value
static unsigned long colorToPixel(Display* display, const Color& color) {
    int screen = DefaultScreen(display);
    Colormap colormap = DefaultColormap(display, screen);
    XColor xcolor;
    
    xcolor.red = color.r * 257;   // Convert 0-255 to 0-65535
    xcolor.green = color.g * 257;
    xcolor.blue = color.b * 257;
    xcolor.flags = DoRed | DoGreen | DoBlue;
    
    XAllocColor(display, colormap, &xcolor);
    return xcolor.pixel;
}

// X11 key mapping
static KeyCode mapX11Key(KeySym keysym) {
    switch (keysym) {
        case XK_Up: return KEY_UP;
        case XK_Down: return KEY_DOWN;
        case XK_Left: return KEY_LEFT;
        case XK_Right: return KEY_RIGHT;
        
        case XK_a: case XK_A: return KEY_A;
        case XK_b: case XK_B: return KEY_B;
        case XK_c: case XK_C: return KEY_C;
        case XK_d: case XK_D: return KEY_D;
        case XK_e: case XK_E: return KEY_E;
        case XK_f: case XK_F: return KEY_F;
        case XK_g: case XK_G: return KEY_G;
        case XK_h: case XK_H: return KEY_H;
        case XK_i: case XK_I: return KEY_I;
        case XK_j: case XK_J: return KEY_J;
        case XK_k: case XK_K: return KEY_K;
        case XK_l: case XK_L: return KEY_L;
        case XK_m: case XK_M: return KEY_M;
        case XK_n: case XK_N: return KEY_N;
        case XK_o: case XK_O: return KEY_O;
        case XK_p: case XK_P: return KEY_P;
        case XK_q: case XK_Q: return KEY_Q;
        case XK_r: case XK_R: return KEY_R;
        case XK_s: case XK_S: return KEY_S;
        case XK_t: case XK_T: return KEY_T;
        case XK_u: case XK_U: return KEY_U;
        case XK_v: case XK_V: return KEY_V;
        case XK_w: case XK_W: return KEY_W;
        case XK_x: case XK_X: return KEY_X;
        case XK_y: case XK_Y: return KEY_Y;
        case XK_z: case XK_Z: return KEY_Z;
        
        case XK_0: return KEY_0;
        case XK_1: return KEY_1;
        case XK_2: return KEY_2;
        case XK_3: return KEY_3;
        case XK_4: return KEY_4;
        case XK_5: return KEY_5;
        case XK_6: return KEY_6;
        case XK_7: return KEY_7;
        case XK_8: return KEY_8;
        case XK_9: return KEY_9;
        
        case XK_F1: return KEY_F1;
        case XK_F2: return KEY_F2;
        case XK_F3: return KEY_F3;
        case XK_F4: return KEY_F4;
        case XK_F5: return KEY_F5;
        case XK_F6: return KEY_F6;
        case XK_F7: return KEY_F7;
        case XK_F8: return KEY_F8;
        case XK_F9: return KEY_F9;
        case XK_F10: return KEY_F10;
        case XK_F11: return KEY_F11;
        case XK_F12: return KEY_F12;
        
        case XK_space: return KEY_SPACE;
        case XK_Return: return KEY_ENTER;
        case XK_Escape: return KEY_ESCAPE;
        case XK_BackSpace: return KEY_BACKSPACE;
        case XK_Tab: return KEY_TAB;
        case XK_Shift_L:
        case XK_Shift_R: return KEY_SHIFT;
        case XK_Control_L:
        case XK_Control_R: return KEY_CONTROL;
        case XK_Alt_L:
        case XK_Alt_R: return KEY_ALT;
        
        case XK_plus:
        case XK_equal: return KEY_PLUS;
        case XK_minus: return KEY_MINUS;
        
        default: return KEY_UNKNOWN;
    }
}

WindowHandle* createWindow(const char* title, int width, int height) {
    WindowHandle* handle = new WindowHandle();
    handle->width = width;
    handle->height = height;
    
    // Open connection to X server
    handle->display = XOpenDisplay(nullptr);
    if (!handle->display) {
        delete handle;
        return nullptr;
    }
    
    int screen = DefaultScreen(handle->display);
    Window root = RootWindow(handle->display, screen);
    
    // Create window
    handle->window = XCreateSimpleWindow(
        handle->display,
        root,
        0, 0,
        width, height,
        1,
        BlackPixel(handle->display, screen),
        BlackPixel(handle->display, screen)
    );
    
    if (!handle->window) {
        XCloseDisplay(handle->display);
        delete handle;
        return nullptr;
    }
    
    // Set window title
    XStoreName(handle->display, handle->window, title);
    
    // Select input events
    XSelectInput(handle->display, handle->window, 
                 ExposureMask | KeyPressMask | KeyReleaseMask | 
                 ButtonPressMask | ButtonReleaseMask | 
                 PointerMotionMask | StructureNotifyMask);
    
    // Handle window close event
    handle->wmDeleteMessage = XInternAtom(handle->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(handle->display, handle->window, &handle->wmDeleteMessage, 1);
    
    // Create graphics context
    handle->gc = XCreateGC(handle->display, handle->window, 0, nullptr);
    
    // Create back buffer for double buffering
    handle->backBuffer = XCreatePixmap(handle->display, handle->window, 
                                       width, height, 
                                       DefaultDepth(handle->display, screen));
    
    // Map window to screen
    XMapWindow(handle->display, handle->window);
    
    // Wait for window to be mapped
    XEvent event;
    do {
        XNextEvent(handle->display, &event);
    } while (event.type != MapNotify);
    
    return handle;
}

void destroyWindow(WindowHandle* window) {
    if (!window) return;
    
    if (window->display) {
        if (window->backBuffer) {
            XFreePixmap(window->display, window->backBuffer);
        }
        
        if (window->gc) {
            XFreeGC(window->display, window->gc);
        }
        
        if (window->window) {
            XDestroyWindow(window->display, window->window);
        }
        
        XCloseDisplay(window->display);
    }
    
    delete window;
}

bool windowShouldClose(WindowHandle* window) {
    if (!window) return true;
    return window->shouldClose;
}

void pollEvents(WindowHandle* window) {
    if (!window || !window->display) return;
    
    // Save previous state
    memcpy(window->prevKeyState, window->keyState, sizeof(window->keyState));
    memcpy(window->prevMouseState, window->mouseState, sizeof(window->mouseState));
    
    window->prevMouseX = window->mouseX;
    window->prevMouseY = window->mouseY;
    window->mouseWheelDelta = 0;
    
    XEvent event;
    while (XPending(window->display) > 0) {
        XNextEvent(window->display, &event);
        
        switch (event.type) {
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == window->wmDeleteMessage) {
                    window->shouldClose = true;
                }
                break;
                
            case KeyPress: {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                KeyCode key = mapX11Key(keysym);
                if (key != KEY_UNKNOWN && key < ::KEY_COUNT) {
                    window->keyState[key] = true;
                }
                if (keysym == XK_Escape) {
                    window->shouldClose = true;
                }
                break;
            }
            
            case KeyRelease: {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                KeyCode key = mapX11Key(keysym);
                if (key != KEY_UNKNOWN && key < ::KEY_COUNT) {
                    window->keyState[key] = false;
                }
                break;
            }
            
            case ButtonPress:
                if (event.xbutton.button == Button1)
                    window->mouseState[MOUSE_LEFT] = true;
                else if (event.xbutton.button == Button2)
                    window->mouseState[MOUSE_MIDDLE] = true;
                else if (event.xbutton.button == Button3)
                    window->mouseState[MOUSE_RIGHT] = true;
                else if (event.xbutton.button == Button4)
                    window->mouseWheelDelta = 1;
                else if (event.xbutton.button == Button5)
                    window->mouseWheelDelta = -1;
                break;
                
            case ButtonRelease:
                if (event.xbutton.button == Button1)
                    window->mouseState[MOUSE_LEFT] = false;
                else if (event.xbutton.button == Button2)
                    window->mouseState[MOUSE_MIDDLE] = false;
                else if (event.xbutton.button == Button3)
                    window->mouseState[MOUSE_RIGHT] = false;
                break;
                
            case MotionNotify:
                window->mouseX = event.xmotion.x;
                window->mouseY = event.xmotion.y;
                break;
        }
    }
    
    // Calculate mouse delta
    window->mouseDeltaX = window->mouseX - window->prevMouseX;
    window->mouseDeltaY = window->mouseY - window->prevMouseY;
    
    // Handle mouse locking
    if (window->mouseLocked) {
        int centerX = window->width / 2;
        int centerY = window->height / 2;
        
        XWarpPointer(window->display, None, window->window, 0, 0, 0, 0, centerX, centerY);
        XFlush(window->display);
        
        window->mouseX = centerX;
        window->mouseY = centerY;
    }
}

void swapBuffers(WindowHandle* window) {
    if (!window || !window->display || !window->gc) return;
    
    // Copy back buffer to window
    XCopyArea(window->display, window->backBuffer, window->window, window->gc,
              0, 0, window->width, window->height, 0, 0);
    
    // Flush the output buffer
    XFlush(window->display);
}

void clearScreen(WindowHandle* window, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    unsigned long pixel = colorToPixel(window->display, color);
    XSetForeground(window->display, window->gc, pixel);
    XFillRectangle(window->display, window->backBuffer, window->gc,
                   0, 0, window->width, window->height);
}

void setDrawColor(WindowHandle* window, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    window->currentColor = colorToPixel(window->display, color);
    XSetForeground(window->display, window->gc, window->currentColor);
}

void drawLine(WindowHandle* window, int x1, int y1, int x2, int y2, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    setDrawColor(window, color);
    XDrawLine(window->display, window->backBuffer, window->gc, x1, y1, x2, y2);
}

void drawRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    setDrawColor(window, color);
    XDrawRectangle(window->display, window->backBuffer, window->gc, x, y, width, height);
}

void drawFilledRectangle(WindowHandle* window, int x, int y, int width, int height, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    setDrawColor(window, color);
    XFillRectangle(window->display, window->backBuffer, window->gc, x, y, width, height);
}

void drawPixel(WindowHandle* window, int x, int y, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    setDrawColor(window, color);
    XDrawPoint(window->display, window->backBuffer, window->gc, x, y);
}

// Helper function for drawing circles using midpoint circle algorithm
static void drawCirclePoints(Display* display, Drawable drawable, GC gc,
                             int xc, int yc, int x, int y) {
    XDrawPoint(display, drawable, gc, xc + x, yc + y);
    XDrawPoint(display, drawable, gc, xc - x, yc + y);
    XDrawPoint(display, drawable, gc, xc + x, yc - y);
    XDrawPoint(display, drawable, gc, xc - x, yc - y);
    XDrawPoint(display, drawable, gc, xc + y, yc + x);
    XDrawPoint(display, drawable, gc, xc - y, yc + x);
    XDrawPoint(display, drawable, gc, xc + y, yc - x);
    XDrawPoint(display, drawable, gc, xc - y, yc - x);
}

void drawCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    setDrawColor(window, color);
    
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    drawCirclePoints(window->display, window->backBuffer, window->gc, 
                    centerX, centerY, x, y);
    
    while (y >= x) {
        x++;
        
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        
        drawCirclePoints(window->display, window->backBuffer, window->gc,
                        centerX, centerY, x, y);
    }
}

void drawFilledCircle(WindowHandle* window, int centerX, int centerY, int radius, const Color& color) {
    if (!window || !window->display || !window->gc) return;
    
    setDrawColor(window, color);
    
    // X11 has XFillArc which is more efficient
    XFillArc(window->display, window->backBuffer, window->gc,
             centerX - radius, centerY - radius,
             radius * 2, radius * 2,
             0, 360 * 64);  // Angles in X11 are in 1/64ths of a degree
}

void delay(uint32_t milliseconds) {
    usleep(milliseconds * 1000);
}

// ============================================================================
// INPUT HANDLING - X11
// ============================================================================

bool keyDown(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return window->keyState[key];
}

bool keyPressed(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return window->keyState[key] && !window->prevKeyState[key];
}

bool keyReleased(WindowHandle* window, KeyCode key) {
    if (!window || key >= KEY_COUNT) return false;
    return !window->keyState[key] && window->prevKeyState[key];
}

bool mouseDown(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return window->mouseState[button];
}

bool mousePressed(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return window->mouseState[button] && !window->prevMouseState[button];
}

bool mouseReleased(WindowHandle* window, MouseButton button) {
    if (!window || button >= MOUSE_BUTTON_COUNT) return false;
    return !window->mouseState[button] && window->prevMouseState[button];
}

void getMousePosition(WindowHandle* window, int& x, int& y) {
    if (!window) {
        x = y = 0;
        return;
    }
    x = window->mouseX;
    y = window->mouseY;
}

void getMouseDelta(WindowHandle* window, int& dx, int& dy) {
    if (!window) {
        dx = dy = 0;
        return;
    }
    dx = window->mouseDeltaX;
    dy = window->mouseDeltaY;
}

void setMousePosition(WindowHandle* window, int x, int y) {
    if (!window || !window->display) return;
    
    XWarpPointer(window->display, None, window->window, 0, 0, 0, 0, x, y);
    XFlush(window->display);
    
    window->mouseX = x;
    window->mouseY = y;
}

void setMouseLocked(WindowHandle* window, bool locked) {
    if (!window || !window->display) return;
    window->mouseLocked = locked;
    
    if (locked) {
        // Hide cursor
        Cursor invisibleCursor;
        Pixmap bitmapNoData;
        XColor black;
        static char noData[] = {0,0,0,0,0,0,0,0};
        black.red = black.green = black.blue = 0;
        
        bitmapNoData = XCreateBitmapFromData(window->display, window->window, noData, 8, 8);
        invisibleCursor = XCreatePixmapCursor(window->display, bitmapNoData, bitmapNoData,
                                             &black, &black, 0, 0);
        XDefineCursor(window->display, window->window, invisibleCursor);
        XFreeCursor(window->display, invisibleCursor);
        XFreePixmap(window->display, bitmapNoData);
    } else {
        XUndefineCursor(window->display, window->window);
    }
}

bool isMouseLocked(WindowHandle* window) {
    if (!window) return false;
    return window->mouseLocked;
}

int getMouseWheelDelta(WindowHandle* window) {
    if (!window) return 0;
    return window->mouseWheelDelta;
}

#endif // USE_X11