#include "graphics.h"
#include <cmath>
#include <vector>

struct Vec3 {
    float x, y, z;
};

// Rotate point around X axis
Vec3 rotateX(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return {
        v.x,
        v.y * c - v.z * s,
        v.y * s + v.z * c
    };
}

// Rotate point around Y axis
Vec3 rotateY(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return {
        v.x * c + v.z * s,
        v.y,
        -v.x * s + v.z * c
    };
}

// Perspective projection
void project(const Vec3& v, int& x2d, int& y2d, float width, float height) {
    float distance = 2.0f;        // camera distance
    float scale = 100.0f;         // zoom factor

    float z = v.z + distance;
    float factor = scale / z;

    x2d = static_cast<int>(v.x * factor + width / 2);
    y2d = static_cast<int>(v.y * factor + height / 2);
}

int main() {
    const int width = 640;
    const int height = 480;

    WindowHandle* window = createWindow("3D Spinning Cube", width, height);
    if (!window) return -1;

    // Cube vertices
    std::vector<Vec3> vertices = {
        {-1, -1, -1},
        { 1, -1, -1},
        { 1,  1, -1},
        {-1,  1, -1},
        {-1, -1,  1},
        { 1, -1,  1},
        { 1,  1,  1},
        {-1,  1,  1}
    };

    // Cube edges (pairs of vertex indices)
    std::vector<std::pair<int,int>> edges = {
        {0,1},{1,2},{2,3},{3,0}, // back face
        {4,5},{5,6},{6,7},{7,4}, // front face
        {0,4},{1,5},{2,6},{3,7}  // connecting edges
    };

    float angle = 0.0f;

    while (!windowShouldClose(window)) {
        pollEvents(window);
        clearScreen(window, Color(0, 0, 0)); // RGB 20, 20, 30

        std::vector<Vec3> transformed;

        // Rotate cube
        for (const auto& v : vertices) {
            Vec3 r = rotateX(v, angle);
            r = rotateY(r, angle * 0.7f);
            transformed.push_back(r);
        }

        // Draw edges
        for (const auto& edge : edges) {
            int x1, y1, x2, y2;

            project(transformed[edge.first], x1, y1, width, height);
            project(transformed[edge.second], x2, y2, width, height);

            drawLine(window, x1, y1, x2, y2, Color(255, 255, 255)); // RGB 0 255 180
        }

        swapBuffers(window);

        angle += 0.01f;
        delay(16); // ~60 FPS
    }

    destroyWindow(window);
    return 0;
}
