#include "graphics.h"
#include <cmath>
#include <vector>
#include <cstdlib>

struct Vec3 {
    float x, y, z;
};

Vec3 rotateX(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { v.x, v.y * c - v.z * s, v.y * s + v.z * c };
}

Vec3 rotateY(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { v.x * c + v.z * s, v.y, -v.x * s + v.z * c };
}

Vec3 rotateZ(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { v.x * c - v.y * s, v.x * s + v.y * c, v.z };
}

void project(const Vec3& v, int& x2d, int& y2d, float width, float height, float zoom) {
    float distance = 4.0f;
    float z = v.z + distance;
    float factor = zoom / z;

    x2d = static_cast<int>(v.x * factor + width / 2);
    y2d = static_cast<int>(v.y * factor + height / 2);
}

struct Star {
    float x, y, z;
};

int main() {
    const int width = 800;
    const int height = 600;

    WindowHandle* window = createWindow("DEMOSCENE CUBE", width, height);
    if (!window) return -1;

    std::vector<Vec3> vertices = {
        {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
        {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}
    };

    std::vector<std::pair<int,int>> edges = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    // Starfield
    const int starCount = 300;
    std::vector<Star> stars;
    for (int i = 0; i < starCount; ++i) {
        stars.push_back({
            ((rand() % 200) - 100) / 10.0f,
            ((rand() % 200) - 100) / 10.0f,
            (rand() % 100) / 10.0f
        });
    }

    float time = 0.0f;

    while (!windowShouldClose(window)) {
        pollEvents(window);

        // Pulsing background
        int bg = static_cast<int>((std::sin(time * 0.5f) + 1.0f) * 20);
        clearScreen(window, Color(bg, 0, bg + 20));

        // --- STARFIELD ---
        for (auto& star : stars) {
            star.z -= 0.05f;
            if (star.z <= 0.1f)
                star.z = 10.0f;

            int sx = static_cast<int>((star.x / star.z) * 200 + width/2);
            int sy = static_cast<int>((star.y / star.z) * 200 + height/2);

            if (sx >= 0 && sx < width && sy >= 0 && sy < height)
                drawPixel(window, sx, sy, Color(255,255,255));
        }

        // Color cycling (rainbow)
        int r = static_cast<int>((std::sin(time) + 1) * 127);
        int g = static_cast<int>((std::sin(time + 2) + 1) * 127);
        int b = static_cast<int>((std::sin(time + 4) + 1) * 127);

        float zoom = 150.0f + std::sin(time) * 50.0f;

        // --- MULTI-CUBE EFFECT ---
        for (int c = 0; c < 3; ++c) {

            float offset = c * 2.5f - 2.5f;

            std::vector<Vec3> transformed;

            for (const auto& v : vertices) {
                Vec3 r3 = rotateX(v, time + c);
                r3 = rotateY(r3, time * 0.7f + c);
                r3 = rotateZ(r3, time * 0.5f);
                r3.x += offset;
                transformed.push_back(r3);
            }

            for (const auto& edge : edges) {
                int x1, y1, x2, y2;
                project(transformed[edge.first], x1, y1, width, height, zoom);
                project(transformed[edge.second], x2, y2, width, height, zoom);
                drawLine(window, x1, y1, x2, y2, Color(r, g, b));
            }
        }

        swapBuffers(window);

        time += 0.02f;
        delay(16);
    }

    destroyWindow(window);
    return 0;
}
