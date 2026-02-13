#include "graphics.h"
#include <cmath>

#ifdef USE_SDL

#include <SDL2/SDL.h>

// Platform-specific window handle structure
struct WindowHandle {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool shouldClose;
    
    WindowHandle() : window(nullptr), renderer(nullptr), shouldClose(false) {}
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
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            window->shouldClose = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                window->shouldClose = true;
            }
        }
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

#endif // USE_SDL

#ifdef USE_WIN32

#include <windows.h>
#include <vector>

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
    
    WindowHandle() : hwnd(nullptr), hdc(nullptr), memDC(nullptr), 
                     memBitmap(nullptr), oldBitmap(nullptr),
                     width(0), height(0), shouldClose(false), 
                     currentColor(RGB(255, 255, 255)) {}
};

// Global window class name
static const char* WINDOW_CLASS_NAME = "LibGraffikWindowClass";
static bool classRegistered = false;

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
            if (wParam == VK_ESCAPE && handle) {
                handle->shouldClose = true;
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
    
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
    
    WindowHandle() : display(nullptr), window(0), gc(nullptr), 
                     backBuffer(0), width(0), height(0), 
                     shouldClose(false), wmDeleteMessage(0),
                     currentColor(0xFFFFFF) {}
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
                 ExposureMask | KeyPressMask | StructureNotifyMask);
    
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
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_Escape) {
                    window->shouldClose = true;
                }
                break;
            }
        }
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

#endif // USE_X11
