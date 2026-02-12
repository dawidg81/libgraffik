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
// Win32 implementation would go here
// For now, this is a placeholder
#error "Win32 backend not yet implemented"
#endif

#ifdef USE_X11
// X11 implementation would go here
// For now, this is a placeholder
#error "X11 backend not yet implemented"
#endif