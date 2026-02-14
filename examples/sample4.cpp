#include "graphics.h"
#include <cmath>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// 8x8 monochrome bitmap font (basic ASCII)
unsigned char font8x8_basic[128][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020 (space)
    { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021 (!)
    { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022 (")
    { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // U+0023 (#)
    { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},   // U+0024 ($)
    { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
    { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},   // U+0026 (&)
    { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027 (')
    { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},   // U+0028 (()
    { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},   // U+0029 ())
    { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},   // U+002A (*)
    { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},   // U+002B (+)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+002C (,)
    { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},   // U+002D (-)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},   // U+002F (/)
    { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},   // U+0030 (0)
    { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
    { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
    { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
    { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
    { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
    { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
    { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
    { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
    { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+003B (;)
    { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},   // U+003C (<)
    { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},   // U+003D (=)
    { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},   // U+003E (>)
    { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},   // U+003F (?)
    { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},   // U+0040 (@)
    { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0041 (A)
    { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0042 (B)
    { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
    { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},   // U+0044 (D)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0045 (E)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},   // U+0046 (F)
    { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},   // U+0047 (G)
    { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0048 (H)
    { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
    { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},   // U+004A (J)
    { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+004B (K)
    { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},   // U+004C (L)
    { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+004D (M)
    { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
    { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
    { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
    { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},   // U+0051 (Q)
    { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},   // U+0052 (R)
    { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},   // U+0053 (S)
    { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0056 (V)
    { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // U+0057 (W)
    { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+0058 (X)
    { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+0059 (Y)
    { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+005A (Z)
    { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},   // U+005B ([)
    { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},   // U+005C (\)
    { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},   // U+005D (])
    { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},   // U+005E (^)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+005F (_)
    { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060 (`)
    { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
    { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},   // U+0062 (b)
    { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},   // U+0063 (c)
    { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},   // U+0064 (d)
    { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},   // U+0065 (e)
    { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},   // U+0066 (f)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0067 (g)
    { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},   // U+0068 (h)
    { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0069 (i)
    { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},   // U+006A (j)
    { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},   // U+006B (k)
    { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+006C (l)
    { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},   // U+006D (m)
    { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},   // U+006E (n)
    { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+006F (o)
    { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},   // U+0070 (p)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},   // U+0071 (q)
    { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},   // U+0072 (r)
    { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},   // U+0073 (s)
    { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0074 (t)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},   // U+0075 (u)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0076 (v)
    { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},   // U+0077 (w)
    { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},   // U+0078 (x)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0079 (y)
    { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},   // U+007A (z)
    { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},   // U+007B ({)
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C (|)
    { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},   // U+007D (})
    { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E (~)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    // U+007F
};

// Helper function to draw text using bitmap font
void drawText(WindowHandle* window, int x, int y, const char* text, Color color) {
    int cursorX = x;
    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        if (c < 32 || c > 126) c = ' '; // Only printable ASCII
        
        int fontIndex = c - 32; // Font array starts at space (32)
        
        for (int row = 0; row < 8; ++row) {
            unsigned char rowData = font8x8_basic[fontIndex][row];
            for (int col = 0; col < 8; ++col) {
                if (rowData & (1 << col)) {
                    drawPixel(window, cursorX + col, y + row, color);
                }
            }
        }
        cursorX += 8; // Move to next character position
    }
}

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
    const float nearPlane = 0.5f;
    
    if (v.z <= nearPlane) return false;
    
    float factor = fov / v.z;
    x2d = static_cast<int>(v.x * factor + width/2);
    y2d = static_cast<int>(v.y * factor + height/2);
    
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
    
    // Generate stars
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
    
    // Generate cubes
    int cubeCount = 2 + (hashCoord(coord.x, coord.y, coord.z, 2) % 4);
    for (int i = 0; i < cubeCount; ++i) {
        Cube cube;
        cube.pos = Vec3(
            chunkOrigin.x + randomFloat(seed) * chunkSize,
            chunkOrigin.y + randomFloat(seed) * chunkSize,
            chunkOrigin.z + randomFloat(seed) * chunkSize
        );
        cube.size = 1.0f + randomFloat(seed) * 4.0f;
        
        uint8_t r = static_cast<uint8_t>(randomFloat(seed) * 100 + 155);
        uint8_t g = static_cast<uint8_t>(randomFloat(seed) * 100 + 155);
        uint8_t b = static_cast<uint8_t>(randomFloat(seed) * 100 + 155);
        cube.color = Color(r, g, b);
        
        chunk.cubes.push_back(cube);
    }
    
    // Generate planets
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

    WindowHandle* window = createWindow("INFINITE SPACE NAVIGATION", width, height);
    if (!window) return -1;

    setMouseLocked(window, true);

    Camera cam;
    cam.position = Vec3(0, 0, -5);
    cam.velocity = Vec3(0, 0, 0);
    cam.yaw = 0;
    cam.pitch = 0;
    cam.yawVelocity = 0;
    cam.pitchVelocity = 0;

    const float moveAccel = 0.01f;
    const float moveDamping = 1.0f;
    const float maxSpeed = 1000.0f;
    const float mouseSensitivity = 0.003f;
    const float mouseSmoothing = 0.1f;
    
    const float viewDistance = 300.0f;
    const int renderRadius = 3;
    float speed = 0.0f;
    float acceleration = 0.0f;

    // Cube vertices
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

        // Mouse look
        int dx, dy;
        getMouseDelta(window, dx, dy);

        cam.yawVelocity = cam.yawVelocity * (1.0f - mouseSmoothing) + dx * mouseSensitivity * mouseSmoothing;
        cam.pitchVelocity = cam.pitchVelocity * (1.0f - mouseSmoothing) + -dy * mouseSensitivity * mouseSmoothing;

        cam.yaw += cam.yawVelocity;
        cam.pitch += cam.pitchVelocity;

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

        // Movement
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

        float inputLen = length(inputVelocity);
        if (inputLen > 0.01f) {
            inputVelocity = normalize(inputVelocity) * moveAccel;
        }

        // Calculate acceleration
        Vec3 oldVelocity = cam.velocity;
        cam.velocity = cam.velocity + inputVelocity;
        cam.velocity = cam.velocity * moveDamping;
        
        speed = length(cam.velocity);
        if (speed > maxSpeed) {
            cam.velocity = normalize(cam.velocity) * maxSpeed;
            speed = maxSpeed;
        }
        
        // Calculate acceleration as change in velocity
        Vec3 accelVec = cam.velocity - oldVelocity;
        acceleration = length(accelVec);
        
        cam.position = cam.position + cam.velocity;

        // Chunk loading
        ChunkCoord currentChunk = worldToChunk(cam.position);
        
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
        
        // Unload distant chunks
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

        // Rendering
        clearScreen(window, Color(0, 0, 5));

        const float fov = 600.0f;
        int starsRendered = 0;
        int cubesRendered = 0;
        int planetsRendered = 0;

        // Draw stars
        for (auto& chunkPair : chunkCache) {
            for (auto& star : chunkPair.second.stars) {
                Vec3 p = star.pos - cam.position;
                
                float dist = length(p);
                if (dist > viewDistance) continue;
                
                p = rotateY(p, -cam.yaw);
                p = rotateX(p, -cam.pitch);

                int sx, sy;
                if (project(p, sx, sy, width, height, fov)) {
                    if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                        float brightness = 1.0f - (dist / viewDistance);
                        uint8_t b = static_cast<uint8_t>(star.brightness * brightness);
                        drawPixel(window, sx, sy, Color(b, b, b));
                        starsRendered++;
                    }
                }
            }
        }

        // Draw cubes
        for (auto& chunkPair : chunkCache) {
            for (auto& cube : chunkPair.second.cubes) {
                Vec3 cubePos = cube.pos - cam.position;
                
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
                    
                    if (v.z > -1.0f) anyVisible = true;
                }

                if (!anyVisible) continue;

                for (auto& e : cubeEdges) {
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

        // Draw planets
        for (auto& chunkPair : chunkCache) {
            for (auto& planet : chunkPair.second.planets) {
                Vec3 p = planet.pos - cam.position;
                
                if (length(p) > viewDistance) continue;
                
                p = rotateY(p, -cam.yaw);
                p = rotateX(p, -cam.pitch);

                int cx, cy;
                if (project(p, cx, cy, width, height, fov)) {
                    float scale = fov / p.z;
                    int radius2D = static_cast<int>(planet.radius * scale);
                    
                    if (radius2D > 1 && radius2D < 500) {
                        drawCircle(window, cx, cy, radius2D, planet.color);
                        planetsRendered++;
                    }
                }
            }
        }

        // Draw HUD with actual text
        char textBuf[256];
        int lineY = 10;
        
        // Position
        snprintf(textBuf, sizeof(textBuf), "Position X:%.1f Y:%.1f Z:%.1f", 
                 cam.position.x, cam.position.y, cam.position.z);
        drawText(window, 10, lineY, textBuf, Color(0, 255, 100));
        lineY += 12;
        
        // Speed
        snprintf(textBuf, sizeof(textBuf), "Speed: %.2f", speed);
        drawText(window, 10, lineY, textBuf, Color(100, 200, 255));
        lineY += 12;
        
        // Acceleration
        snprintf(textBuf, sizeof(textBuf), "Accel: %.4f", acceleration);
        drawText(window, 10, lineY, textBuf, Color(255, 200, 100));
        lineY += 12;
        
        // Velocity components
        snprintf(textBuf, sizeof(textBuf), "Velocity X:%.2f Y:%.2f Z:%.2f", 
                 cam.velocity.x, cam.velocity.y, cam.velocity.z);
        drawText(window, 10, lineY, textBuf, Color(200, 150, 255));
        lineY += 12;
        
        // Camera rotation
        snprintf(textBuf, sizeof(textBuf), "Yaw:%.2f Pitch:%.2f", cam.yaw, cam.pitch);
        drawText(window, 10, lineY, textBuf, Color(255, 255, 100));
        lineY += 12;
        
        // Render stats
        snprintf(textBuf, sizeof(textBuf), "Stars:%d Cubes:%d Planets:%d", 
                 starsRendered, cubesRendered, planetsRendered);
        drawText(window, 10, lineY, textBuf, Color(100, 255, 255));
        lineY += 12;
        
        // Chunks loaded
        snprintf(textBuf, sizeof(textBuf), "Chunks:%d FPS:%d", (int)chunkCache.size(), fps);
        drawText(window, 10, lineY, textBuf, Color(255, 100, 255));

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