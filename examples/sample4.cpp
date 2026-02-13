#include "graphics.h"
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Vec3 {
    float x, y, z;
};

Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
Vec3 operator*(const Vec3& a, float s) { return {a.x*s,a.y*s,a.z*s}; }

float length(const Vec3& v) {
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vec3 normalize(const Vec3& v) {
    float l = length(v);
    if (l == 0) return {0,0,0};
    return {v.x/l, v.y/l, v.z/l};
}

Vec3 rotateX(const Vec3& v, float a) {
    float c = std::cos(a), s = std::sin(a);
    return { v.x, v.y*c - v.z*s, v.y*s + v.z*c };
}

Vec3 rotateY(const Vec3& v, float a) {
    float c = std::cos(a), s = std::sin(a);
    return { v.x*c + v.z*s, v.y, -v.x*s + v.z*c };
}

// Camera
struct Camera {
    Vec3 position;
    float yaw;
    float pitch;
};

void project(const Vec3& v, int& x2d, int& y2d,
             int width, int height, float fov)
{
    if (v.z <= 0.1f) return;

    float factor = fov / v.z;
    x2d = static_cast<int>(v.x * factor + width/2);
    y2d = static_cast<int>(v.y * factor + height/2);
}

struct Star {
    Vec3 pos;
};

struct Cube {
    Vec3 pos;
    float size;
};

struct Planet {
    Vec3 pos;
    float radius;
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int width = 1280;
    const int height = 720;

    WindowHandle* window = createWindow("SAMPLE4 - SPACE NAVIGATION", width, height);
    if (!window) return -1;

    setMouseLocked(window, true);

    Camera cam;
    cam.position = {0,0,-5};
    cam.yaw = 0;
    cam.pitch = 0;

    const float moveSpeed = 0.3f;
    const float mouseSensitivity = 0.002f;

    // ----- STARFIELD -----
    const int starCount = 1500;
    std::vector<Star> stars;
    for (int i = 0; i < starCount; ++i) {
        stars.push_back({
            {
                (rand()%2000 - 1000) / 10.0f,
                (rand()%2000 - 1000) / 10.0f,
                (rand()%2000 - 1000) / 10.0f
            }
        });
    }

    // ----- CUBES -----
    std::vector<Vec3> cubeVertices = {
        {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
        {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}
    };

    std::vector<std::pair<int,int>> cubeEdges = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    std::vector<Cube> cubes;
    for (int i = 0; i < 40; ++i) {
        cubes.push_back({
            {
                (rand()%2000 - 1000) / 10.0f,
                (rand()%2000 - 1000) / 10.0f,
                (rand()%2000 - 1000) / 10.0f
            },
            (rand()%50)/10.0f + 1.0f
        });
    }

    // ----- PLANETS -----
    std::vector<Planet> planets;
    for (int i = 0; i < 5; ++i) {
        planets.push_back({
            {
                (rand()%3000 - 1500) / 10.0f,
                (rand()%3000 - 1500) / 10.0f,
                (rand()%3000 - 1500) / 10.0f
            },
            (rand()%200)/10.0f + 5.0f
        });
    }

    while (!windowShouldClose(window)) {

        pollEvents(window);

        // ----- MOUSE LOOK -----
        int dx, dy;
        getMouseDelta(window, dx, dy);

        cam.yaw   += dx * mouseSensitivity;
        cam.pitch += dy * mouseSensitivity;

        if (cam.pitch > 1.5f) cam.pitch = 1.5f;
        if (cam.pitch < -1.5f) cam.pitch = -1.5f;

        // Forward vector
        Vec3 forward = {
            std::sin(cam.yaw),
            0,
            std::cos(cam.yaw)
        };

        Vec3 right = {
            std::cos(cam.yaw),
            0,
            -std::sin(cam.yaw)
        };

        // ----- MOVEMENT -----
        if (keyDown(window, KEY_UP))
            cam.position = cam.position + forward * moveSpeed;
        if (keyDown(window, KEY_DOWN))
            cam.position = cam.position - forward * moveSpeed;
        if (keyDown(window, KEY_LEFT))
            cam.position = cam.position - right * moveSpeed;
        if (keyDown(window, KEY_RIGHT))
            cam.position = cam.position + right * moveSpeed;

        clearScreen(window, Color(0,0,10));

        float fov = 500.0f;

        // ----- DRAW STARS -----
        for (auto& star : stars) {

            Vec3 p = star.pos - cam.position;
            p = rotateY(p, -cam.yaw);
            p = rotateX(p, -cam.pitch);

            if (p.z > 0) {
                int sx, sy;
                project(p, sx, sy, width, height, fov);
                if (sx>=0 && sx<width && sy>=0 && sy<height)
                    drawPixel(window, sx, sy, Color(255,255,255));
            }
        }

        // ----- DRAW CUBES -----
        for (auto& cube : cubes) {

            std::vector<Vec3> transformed;

            for (auto v : cubeVertices) {
                v = v * cube.size;
                v = v + cube.pos;
                v = v - cam.position;
                v = rotateY(v, -cam.yaw);
                v = rotateX(v, -cam.pitch);
                transformed.push_back(v);
            }

            for (auto& e : cubeEdges) {
                if (transformed[e.first].z > 0 &&
                    transformed[e.second].z > 0) {

                    int x1,y1,x2,y2;
                    project(transformed[e.first], x1,y1,width,height,fov);
                    project(transformed[e.second],x2,y2,width,height,fov);
                    drawLine(window,x1,y1,x2,y2,Color(0,255,200));
                }
            }
        }

        // ----- DRAW PLANETS (wireframe sphere imitation) -----
        for (auto& planet : planets) {

            Vec3 p = planet.pos - cam.position;
            p = rotateY(p, -cam.yaw);
            p = rotateX(p, -cam.pitch);

            if (p.z <= 0) continue;

            int cx, cy;
            project(p, cx, cy, width, height, fov);

            float scale = fov / p.z;
            int radius2D = static_cast<int>(planet.radius * scale);

            drawCircle(window, cx, cy, radius2D, Color(100,100,255));
        }

        swapBuffers(window);
        delay(16);
    }

    destroyWindow(window);
    return 0;
}
