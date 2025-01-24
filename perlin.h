#ifndef PERLIN_H
#define PERLIN_H

#include <string>
#include <vector>

unsigned a_mult, b_mult, c_mult, offset_x, offset_y, offset_z;
const int MAP_WIDTH = 4000;
const int MAP_HEIGHT = 4000;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 400;
float player_speed = 2.0f;

struct biome {
    std::string name;
    Color color;
};

std::vector<biome> biomes = {
    {"Valley", {120, 200, 120, 255}},
    {"Beach", {230, 230, 110, 255}},
    {"Snow", {200, 200, 230, 255}},
    {"Ocean", {0, 105, 148, 255}},
    {"Forest", {60, 140, 60, 255}}
};

biome get_biome(unsigned char temperature, unsigned char humidity) {
    if (temperature >= 131 && temperature <= 160) { return biomes[0]; }
    if (temperature >= 123 && temperature <= 130) { return biomes[1]; }
    if (temperature >= 71 && temperature <= 122) { return biomes[3]; }
    if (temperature <= 70) { return biomes[2]; }
    if (temperature >= 161) { return biomes[4]; }
}




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

unsigned char perlin_generate(int x, int y) {
    float val = 0;
    float freq = 1;
    float amp = 1.1;

    for (int i = 0; i < 12; i++) {
        val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
        freq *= 2;
        amp /= 2;
    }

    val *= 1.2;

    if (val > 1.0f) val = 1.0f;
    else if (val < -1.0f) val = -1.0f;

    int value = (val + 1) * 0.5f * 255;
    return value;
}



Texture2D GenerateMap(int** map, unsigned int seed) {
    Image image = GenImageColor(MAP_WIDTH, MAP_HEIGHT, WHITE);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            unsigned char temperature = perlin_generate(x, y);
            //unsigned char humidity = perlin_generate(x, y);

            biome biome_current = get_biome(temperature, temperature);
            ImageDrawPixel(&image, x, y, biome_current.color);
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    return texture;
}





#endif //PERLIN_H
