#include "raylib.h"
#include <cstdint>
#include <cstdlib>
#include <ctime>

const int MAP_WIDTH = 2000;
const int MAP_HEIGHT = 2000;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1200;

const int GRID_SIZE = 400;


unsigned a_mult, b_mult, c_mult;

Vector2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= a_mult;  // 3284157443;

    b ^= a << s | a >> w - s;
    b *= b_mult;  // 1911520717;

    a ^= b << s | b >> w - s;
    a *= c_mult;  // 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    Vector2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    Vector2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

// Sample Perlin noise at coordinates x, y
float perlin(float x, float y) {

    // Determine grid cell corner coordinates
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Compute and interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float ix0 = interpolate(n0, n1, sx);

    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    float ix1 = interpolate(n0, n1, sx);

    // Final step: interpolate between the two previously interpolated values, now in y
    float value = interpolate(ix0, ix1, sy);

    return value;
}

Color color_transform(int r, int g, int b) {
    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 255;
    return color;
}

Texture2D GenerateMap(int** map, unsigned int seed) {
    Image image = GenImageColor(MAP_WIDTH, MAP_HEIGHT, WHITE);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            float val = 0;

            float freq = 1;
            float amp = 1.1;

            for (int i = 0; i < 12; i++) {
                val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
                freq *= 2;
                amp /= 2;
            }

            val *= 1.2;

            if (val > 1.0f)
                val = 1.0f;
            else if (val < -1.0f)
                val = -1.0f;

            int color = (int)(((val + 1.0f) * 0.5f) * 255);

            if (color >= 131 && color <= 160) { map[y][x] = 1; } // green
            else if (color >= 123 && color <= 130) { map[y][x] = 2; } // yellow
            else if (color >= 71 && color <= 122) { map[y][x] = 3; } // blue
            else if (color >= 0 && color <= 70) { map[y][x] = 4; } // white
            else if (color >= 161 && color <= 255) { map[y][x] = 5; } // dark green

            Color color_now;
            switch (map[y][x]) {
                case 1: color_now = color_transform(color * 0.2, 220, color * 0.3);
                break;
                case 2: color_now = color_transform(255 - color * 0.1, 255 - color * 0.1, color * 0.3);
                break;
                case 3: color_now = color_transform(color * 0.4, color * 0.6, 200);
                break;
                case 4: color_now = color_transform(255 - color * 0.9, 255, 255);
                break;
                case 5: color_now = color_transform(color * 0.1, 255 - color * 0.6, color * 0.2);
                break;
                default: color_now = color_transform(0,0,0);
                break;
            }

            ImageDrawPixel(&image, x, y, color_now);
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    return texture;
}



int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Texture Caching Example");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();
    unsigned int seed = time(nullptr);
    srand(seed);

    a_mult = rand();
    b_mult = rand();
    c_mult = rand();

    // Создание карты в динамической памяти
    int** map = new int*[MAP_HEIGHT];
    for (int i = 0; i < MAP_HEIGHT; i++) {
        map[i] = new int[MAP_WIDTH];
    }

    // Генерируем текстуру карты
    Texture2D mapTexture = GenerateMap(map, seed);;

    Vector2 cameraOffset = { 0, 0 };

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT)) cameraOffset.x += 10;
        if (IsKeyDown(KEY_LEFT)) cameraOffset.x -= 10;
        if (IsKeyDown(KEY_DOWN)) cameraOffset.y += 10;
        if (IsKeyDown(KEY_UP)) cameraOffset.y -= 10;

        // Ограничиваем смещение камеры
        if (cameraOffset.x < 0) cameraOffset.x = 0;
        if (cameraOffset.y < 0) cameraOffset.y = 0;
        if (cameraOffset.x > MAP_WIDTH - SCREEN_WIDTH) cameraOffset.x = MAP_WIDTH - SCREEN_WIDTH;
        if (cameraOffset.y > MAP_HEIGHT - SCREEN_HEIGHT) cameraOffset.y = MAP_HEIGHT - SCREEN_HEIGHT;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Рисуем текстуру карты, сдвигая её по камере
        DrawTextureEx(mapTexture, { -cameraOffset.x, -cameraOffset.y }, 0.0f, 1.0f, WHITE);

        DrawText("Use arrow keys to move", 10, 10, 20, BLACK);
        EndDrawing();
    }

    // Освобождение памяти карты
    for (int i = 0; i < MAP_HEIGHT; i++) {
        delete[] map[i];
    }
    delete[] map;

    UnloadTexture(mapTexture);
    CloseWindow();

    return 0;
}