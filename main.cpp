#include "graphics.h"
#include <cmath>

int main(int argc, char* argv[]) {
    // Create a window
    WindowHandle* window = createWindow("Graphics Demo - SDL Backend", 800, 600);
    
    if (!window) {
        return -1;
    }
    
    // Animation variables
    float time = 0.0f;
    const float PI = 3.14159265359f;
    
    // Main loop
    while (!windowShouldClose(window)) {
        // Handle events
        pollEvents(window);
        
        // Clear screen with a dark background
        clearScreen(window, Color(20, 20, 30));
        
        // Draw a grid
        Color gridColor(50, 50, 70);
        for (int x = 0; x < 800; x += 50) {
            drawLine(window, x, 0, x, 600, gridColor);
        }
        for (int y = 0; y < 600; y += 50) {
            drawLine(window, 0, y, 800, y, gridColor);
        }
        
        // Draw some static shapes
        drawFilledRectangle(window, 50, 50, 100, 80, Color(255, 100, 100));
        drawRectangle(window, 48, 48, 104, 84, Color(255, 150, 150));
        
        drawFilledCircle(window, 250, 90, 40, Color(100, 255, 100));
        drawCircle(window, 250, 90, 42, Color(150, 255, 150));
        
        // Animated rotating line
        int centerX = 400;
        int centerY = 300;
        int lineLength = 150;
        int x2 = centerX + static_cast<int>(lineLength * cos(time));
        int y2 = centerY + static_cast<int>(lineLength * sin(time));
        
        drawLine(window, centerX, centerY, x2, y2, Color(255, 255, 100, 255));
        drawFilledCircle(window, centerX, centerY, 5, Color(255, 200, 0));
        
        // Animated bouncing circle
        int bounceX = 600;
        int bounceY = 300 + static_cast<int>(100 * sin(time * 2));
        drawFilledCircle(window, bounceX, bounceY, 30, Color(100, 200, 255));

        int prevX = bounceX + bounceX + time * 20;
        int prevY = 300 + static_cast<int>(100 * sin(bounceX + time * 20 * 2));

        for (int i = bounceX + time * 20; i > bounceX; i--) {
            int x = bounceX + bounceX + time * 20 - i;
            int y = 300 + static_cast<int>(100 * sin(i * 2));
            // drawPixel(window, x, y, Color(255, 0, 0));
            drawLine(window, prevX, prevY, x, y, Color(255, 0, 0));
            prevX = x;
            prevY = y;
        }
        
        // Draw multiple small circles in a pattern
        for (int i = 0; i < 8; i++) {
            float angle = (time + i * PI / 4);
            int px = 400 + static_cast<int>(100 * cos(angle));
            int py = 300 + static_cast<int>(100 * sin(angle));
            
            Color particleColor(
                static_cast<uint8_t>(128 + 127 * sin(time + i)),
                static_cast<uint8_t>(128 + 127 * sin(time + i + PI * 2 / 3)),
                static_cast<uint8_t>(128 + 127 * sin(time + i + PI * 4 / 3))
            );
            
            drawFilledCircle(window, px, py, 8, particleColor);
        }
        
        // Draw some text-like pixel art (a simple "HI" pattern)
        Color pixelColor(255, 255, 255);
        // Letter H
        for (int y = 0; y < 20; y++) {
            drawPixel(window, 50, 500 + y, pixelColor);
            drawPixel(window, 70, 500 + y, pixelColor);
        }
        for (int x = 50; x <= 70; x++) {
            drawPixel(window, x, 510, pixelColor);
        }
        
        // Letter I
        for (int y = 0; y < 20; y++) {
            drawPixel(window, 90, 500 + y, pixelColor);
        }
        for (int x = 85; x <= 95; x++) {
            drawPixel(window, x, 500, pixelColor);
            drawPixel(window, x, 519, pixelColor);
        }
        
        // Draw a gradient effect using rectangles
        for (int i = 0; i < 50; i++) {
            uint8_t colorValue = static_cast<uint8_t>(i * 5);
            drawFilledRectangle(window, 650 + i * 3, 450, 3, 100, 
                              Color(colorValue, 0, 255 - colorValue));
        }
        
        // Present the rendered frame
        swapBuffers(window);
        
        // Update animation
        time += 0.02f;
        
        // Small delay to control frame rate (~60 FPS)
        delay(16);
    }
    
    // Cleanup
    destroyWindow(window);
    
    return 0;
}