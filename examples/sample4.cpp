#include "graphics.h"
#include <cmath>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>

struct Vec3 {
    float x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.x+b.x, a.y+b.y, a.z+b.z); }
Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.x-b.x, a.y-b.y, a.z-b.z); }
Vec3 operator*(const Vec3& a, float s) { return Vec3(a.x*s, a.y*s, a.z*s); }

float length(const Vec3& v) {
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vec3 normalize(const Vec3& v) {
    float l = length(v);
    if (l == 0) return Vec3(0,0,0);
    return Vec3(v.x/l, v.y/l, v.z/l);
}

Vec3 rotateX(const Vec3& v, float a) {
    float c = std::cos(a), s = std::sin(a);
    return Vec3(v.x, v.y*c - v.z*s, v.y*s + v.z*c);
}

Vec3 rotateY(const Vec3& v, float a) {
    float c = std::cos(a), s = std::sin(a);
    return Vec3(v.x*c + v.z*s, v.y, -v.x*s + v.z*c);
}

// Smooth camera with interpolation
struct Camera {
    Vec3 position;
    Vec3 velocity;
    float yaw;
    float pitch;
    float yawVelocity;
    float pitchVelocity;
};

// Better projection with near plane clipping
bool project(const Vec3& v, int& x2d, int& y2d, int width, int height, float fov) {
    const float nearPlane = 0.5f;  // Prevent rendering too close objects
    
    if (v.z <= nearPlane) return false;
    
    float factor = fov / v.z;
    x2d = static_cast<int>(v.x * factor + width/2);
    y2d = static_cast<int>(v.y * factor + height/2);
    
    // Check if within screen bounds (with margin)
    return (x2d >= -100 && x2d < width + 100 && y2d >= -100 && y2d < height + 100);
}

struct Star {
    Vec3 pos;
    uint8_t brightness;
};

struct Cube {
    Vec3 pos;
    float size;
    Color color;
};

struct Planet {
    Vec3 pos;
    float radius;
    Color color;
};

// Chunk-based world system for infinite generation
struct ChunkCoord {
    int x, y, z;
    
    bool operator<(const ChunkCoord& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

struct Chunk {
    std::vector<Star> stars;
    std::vector<Cube> cubes;
    std::vector<Planet> planets;
};

// Deterministic random based on chunk coordinates
unsigned int hashCoord(int x, int y, int z, int seed) {
    unsigned int h = seed;
    h ^= x * 374761393;
    h ^= y * 668265263;
    h ^= z * 1274126177;
    h = (h ^ (h >> 16)) * 0x85ebca6b;
    h = (h ^ (h >> 13)) * 0xc2b2ae35;
    h = h ^ (h >> 16);
    return h;
}

float randomFloat(unsigned int& seed) {
    seed = seed * 1103515245 + 12345;
    return ((seed / 65536) % 32768) / 32768.0f;
}

Chunk generateChunk(const ChunkCoord& coord) {
    Chunk chunk;
    
    unsigned int seed = hashCoord(coord.x, coord.y, coord.z, 12345);
    
    const float chunkSize = 100.0f;
    Vec3 chunkOrigin(coord.x * chunkSize, coord.y * chunkSize, coord.z * chunkSize);
    
    // Generate stars (lots of them)
    int starCount = 80 + (hashCoord(coord.x, coord.y, coord.z, 1) % 40);
    for (int i = 0; i < starCount; ++i) {
        Star star;
        star.pos = Vec3(
            chunkOrigin.x + randomFloat(seed) * chunkSize,
            chunkOrigin.y + randomFloat(seed) * chunkSize,
            chunkOrigin.z + randomFloat(seed) * chunkSize
        );
        star.brightness = 128 + static_cast<uint8_t>(randomFloat(seed) * 127);
        chunk.stars.push_back(star);
    }
    
    // Generate cubes (fewer)
    int cubeCount = 2 + (hashCoord(coord.x, coord.y, coord.z, 2) % 4);
    for (int i = 0; i < cubeCount; ++i) {
        Cube cube;
        cube.pos = Vec3(
            chunkOrigin.x + randomFloat(seed) * chunkSize,
            chunkOrigin.y + randomFloat(seed) * chunkSize,
            chunkOrigin.z + randomFloat(seed) * chunkSize
        );
        cube.size = 1.0f + randomFloat(seed) * 4.0f;
        
        // Varied colors
        uint8_t r = static_cast<uint8_t>(randomFloat(seed) * 100 + 155);
        uint8_t g = static_cast<uint8_t>(randomFloat(seed) * 100 + 155);
        uint8_t b = static_cast<uint8_t>(randomFloat(seed) * 100 + 155);
        cube.color = Color(r, g, b);
        
        chunk.cubes.push_back(cube);
    }
    
    // Generate planets (rare)
    if (randomFloat(seed) < 0.3f) {
        Planet planet;
        planet.pos = Vec3(
            chunkOrigin.x + randomFloat(seed) * chunkSize,
            chunkOrigin.y + randomFloat(seed) * chunkSize,
            chunkOrigin.z + randomFloat(seed) * chunkSize
        );
        planet.radius = 5.0f + randomFloat(seed) * 15.0f;
        
        uint8_t r = static_cast<uint8_t>(randomFloat(seed) * 150 + 105);
        uint8_t g = static_cast<uint8_t>(randomFloat(seed) * 150 + 105);
        uint8_t b = static_cast<uint8_t>(randomFloat(seed) * 150 + 105);
        planet.color = Color(r, g, b);
        
        chunk.planets.push_back(planet);
    }
    
    return chunk;
}

ChunkCoord worldToChunk(const Vec3& pos) {
    const float chunkSize = 100.0f;
    return ChunkCoord{
        static_cast<int>(std::floor(pos.x / chunkSize)),
        static_cast<int>(std::floor(pos.y / chunkSize)),
        static_cast<int>(std::floor(pos.z / chunkSize))
    };
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int width = 1920;
    const int height = 1080;

    WindowHandle* window = createWindow("SAMPLE4 - INFINITE SPACE NAVIGATION", width, height);
    if (!window) return -1;

    setMouseLocked(window, true);

    Camera cam;
    cam.position = Vec3(0, 0, -5);
    cam.velocity = Vec3(0, 0, 0);
    cam.yaw = 0;
    cam.pitch = 0;
    cam.yawVelocity = 0;
    cam.pitchVelocity = 0;

    const float moveAccel = 0.01f; // 0.8f
    const float moveDamping = 1.0f; // 0.88f
    const float maxSpeed = 10.0f; // 1.5f
    const float mouseSensitivity = 0.003f; // 0.003f
    const float mouseSmoothing = 0.3f; // 0.3f
    
    const float viewDistance = 300.0f; // 300.0f
    const int renderRadius = 1; // Chunks to render in each direction; default = 3
    float speed = 0.0f;

    // Cube vertices (shared)
    std::vector<Vec3> cubeVertices = {
        Vec3(-1,-1,-1), Vec3(1,-1,-1), Vec3(1,1,-1), Vec3(-1,1,-1),
        Vec3(-1,-1,1),  Vec3(1,-1,1),  Vec3(1,1,1),  Vec3(-1,1,1)
    };

    std::vector<std::pair<int,int>> cubeEdges = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    // Chunk cache
    std::map<ChunkCoord, Chunk> chunkCache;
    
    // FPS counter
    int frameCount = 0;
    float fpsTimer = 0;
    int fps = 0;

    while (!windowShouldClose(window)) {
        pollEvents(window);

        // ----- SMOOTH MOUSE LOOK -----
        int dx, dy;
        getMouseDelta(window, dx, dy);

        cam.yawVelocity = cam.yawVelocity * (1.0f - mouseSmoothing) + dx * mouseSensitivity * mouseSmoothing;
        cam.pitchVelocity = cam.pitchVelocity * (1.0f - mouseSmoothing) + -dy * mouseSensitivity * mouseSmoothing;

        cam.yaw += cam.yawVelocity;
        cam.pitch += cam.pitchVelocity;

        // Clamp pitch
        const float maxPitch = 1.5f;
        if (cam.pitch > maxPitch) cam.pitch = maxPitch;
        if (cam.pitch < -maxPitch) cam.pitch = -maxPitch;

        // Direction vectors
        Vec3 forward(
            std::sin(cam.yaw) * std::cos(cam.pitch),
            -std::sin(cam.pitch),
            std::cos(cam.yaw) * std::cos(cam.pitch)
        );
        
        Vec3 right(
            std::cos(cam.yaw),
            0,
            -std::sin(cam.yaw)
        );
        
        Vec3 up(0, 1, 0);

        // ----- SMOOTH MOVEMENT -----
        Vec3 inputVelocity(0, 0, 0);
        
        if (keyDown(window, KEY_W) || keyDown(window, KEY_UP))
            inputVelocity = inputVelocity + forward;
        if (keyDown(window, KEY_S) || keyDown(window, KEY_DOWN))
            inputVelocity = inputVelocity - forward;
        if (keyDown(window, KEY_A) || keyDown(window, KEY_LEFT))
            inputVelocity = inputVelocity - right;
        if (keyDown(window, KEY_D) || keyDown(window, KEY_RIGHT))
            inputVelocity = inputVelocity + right;
        if (keyDown(window, KEY_E))
            inputVelocity = inputVelocity + up;
        if (keyDown(window, KEY_Q))
            inputVelocity = inputVelocity - up;

        // Normalize input to prevent faster diagonal movement
        float inputLen = length(inputVelocity);
        if (inputLen > 0.01f) {
            inputVelocity = normalize(inputVelocity) * moveAccel;
        }

        // Apply acceleration
        cam.velocity = cam.velocity + inputVelocity;
        
        // Apply damping
        cam.velocity = cam.velocity * moveDamping;
        
        // Clamp to max speed
        speed = length(cam.velocity);
        if (speed > maxSpeed) {
            cam.velocity = normalize(cam.velocity) * maxSpeed;
        }
        
        // Update position
        cam.position = cam.position + cam.velocity;

        // ----- CHUNK LOADING/UNLOADING -----
        ChunkCoord currentChunk = worldToChunk(cam.position);
        
        // Load nearby chunks
        for (int x = -renderRadius; x <= renderRadius; ++x) {
            for (int y = -renderRadius; y <= renderRadius; ++y) {
                for (int z = -renderRadius; z <= renderRadius; ++z) {
                    ChunkCoord coord{currentChunk.x + x, currentChunk.y + y, currentChunk.z + z};
                    
                    if (chunkCache.find(coord) == chunkCache.end()) {
                        chunkCache[coord] = generateChunk(coord);
                    }
                }
            }
        }
        
        // Unload distant chunks (keep cache from growing too large)
        std::vector<ChunkCoord> toRemove;
        for (auto& pair : chunkCache) {
            int dx = pair.first.x - currentChunk.x;
            int dy = pair.first.y - currentChunk.y;
            int dz = pair.first.z - currentChunk.z;
            
            if (std::abs(dx) > renderRadius + 1 || 
                std::abs(dy) > renderRadius + 1 || 
                std::abs(dz) > renderRadius + 1) {
                toRemove.push_back(pair.first);
            }
        }
        
        for (auto& coord : toRemove) {
            chunkCache.erase(coord);
        }

        // ----- RENDERING -----
        clearScreen(window, Color(0, 0, 5));

        const float fov = 600.0f;
        int starsRendered = 0;
        int cubesRendered = 0;
        int planetsRendered = 0;

        // ----- DRAW STARS -----
        for (auto& chunkPair : chunkCache) {
            for (auto& star : chunkPair.second.stars) {
                Vec3 p = star.pos - cam.position;
                
                // Distance culling
                float dist = length(p);
                if (dist > viewDistance) continue;
                
                p = rotateY(p, -cam.yaw);
                p = rotateX(p, -cam.pitch);

                int sx, sy;
                if (project(p, sx, sy, width, height, fov)) {
                    if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                        // Dimmer stars when far away
                        float brightness = 1.0f - (dist / viewDistance);
                        uint8_t b = static_cast<uint8_t>(star.brightness * brightness);
                        drawPixel(window, sx, sy, Color(b, b, b));
                        starsRendered++;
                    }
                }
            }
        }

        // ----- DRAW CUBES -----
        for (auto& chunkPair : chunkCache) {
            for (auto& cube : chunkPair.second.cubes) {
                Vec3 cubePos = cube.pos - cam.position;
                
                // Distance culling
                if (length(cubePos) > viewDistance) continue;

                std::vector<Vec3> transformed;
                bool anyVisible = false;

                for (auto v : cubeVertices) {
                    v = v * cube.size;
                    v = v + cube.pos;
                    v = v - cam.position;
                    v = rotateY(v, -cam.yaw);
                    v = rotateX(v, -cam.pitch);
                    transformed.push_back(v);
                    
                    if (v.z > -1.0f) anyVisible = true; // 0.5
                }

                // Only render if at least one vertex is visible
                if (!anyVisible) continue;

                for (auto& e : cubeEdges) {
                    // Only draw edge if both vertices are in front of camera
                    if (transformed[e.first].z > 0.5f && transformed[e.second].z > 0.5f) {
                        int x1, y1, x2, y2;
                        if (project(transformed[e.first], x1, y1, width, height, fov) &&
                            project(transformed[e.second], x2, y2, width, height, fov)) {
                            drawLine(window, x1, y1, x2, y2, cube.color);
                        }
                    }
                }
                cubesRendered++;
            }
        }

        // ----- DRAW PLANETS -----
        for (auto& chunkPair : chunkCache) {
            for (auto& planet : chunkPair.second.planets) {
                Vec3 p = planet.pos - cam.position;
                
                // Distance culling
                if (length(p) > viewDistance) continue;
                
                p = rotateY(p, -cam.yaw);
                p = rotateX(p, -cam.pitch);

                int cx, cy;
                if (project(p, cx, cy, width, height, fov)) {
                    float scale = fov / p.z;
                    int radius2D = static_cast<int>(planet.radius * scale);
                    
                    // Only draw if reasonable size
                    if (radius2D > 1 && radius2D < 500) {
                        drawCircle(window, cx, cy, radius2D, planet.color);
                        planetsRendered++;
                    }
                }
            }
        }

        // ----- DEBUG INFO -----
        // Position text (top-left)
        char posText[128];
        snprintf(posText, sizeof(posText), "X:%.1f Y:%.1f Z:%.1f", 
                 cam.position.x, cam.position.y, cam.position.z);
        
        // Draw simple text using rectangles
        int textY = 10;
        for (int i = 0; posText[i] != '\0'; ++i) {
            drawFilledRectangle(window, 10 + i * 8, textY, 6, 10, Color(0, 255, 0, 128));
        }
        
        // Render stats
        snprintf(posText, sizeof(posText), "Stars:%d Cubes:%d Planets:%d Chunks:%d", 
                 starsRendered, cubesRendered, planetsRendered, (int)chunkCache.size());
        textY = 25;
        for (int i = 0; posText[i] != '\0'; ++i) {
            drawFilledRectangle(window, 10 + i * 8, textY, 6, 10, Color(0, 200, 200, 128));
        }
        
        textY = 40;
        for (int i = 0; speed * 3 > i; i++) {
            drawFilledRectangle(window, 10 + i * 8, textY, 6, 10, Color(0, 200, 200, 128));
        }

        swapBuffers(window);
        delay(16);
        
        // FPS counter
        frameCount++;
        fpsTimer += 16;
        if (fpsTimer >= 1000) {
            fps = frameCount;
            frameCount = 0;
            fpsTimer = 0;
        }
    }

    destroyWindow(window);
    return 0;
}