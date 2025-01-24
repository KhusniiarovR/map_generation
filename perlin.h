#ifndef PERLIN_H
#define PERLIN_H

#include <string>
#include <vector>

#include "perlin.h"

unsigned a_mult, b_mult, c_mult, offset_x, offset_y, offset_z, offset_a;
const int MAP_WIDTH = 2000;
const int MAP_HEIGHT = 2000;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 200;
int map_multiplier = 1;
float player_speed = 20.0f;
Color map_shader_collection[3] = {WHITE, {60, 60, 110, 255}, {100, 50, 20, 255}};
Color map_shader = map_shader_collection[0];

struct biome {
    std::string name;
    Color color;
};

std::vector<biome> biomes = {
    {"Unknown", {255, 0, 0, 255}}, // 0
    {"Beach", {230, 230, 110, 255}}, // 1
    {"Snow", {220, 220, 235, 255}}, // 2
    {"Ocean", {0, 105, 148, 255}}, // 3
    {"Forest", {60, 140, 60, 255}}, // 4
    {"Taiga", {190, 190, 200, 255}}, // 5
    {"Gray biome", {100, 100, 100, 255}}, // 6
    {"Desert", {200, 200, 60, 255}}, // 7
    {"Volcano", {35, 30, 30, 255}}, // 8
    {"Valley", {120, 200, 120, 255}}, // 9
    {"Volcano fire side", {70, 25, 20, 255}}, // 10
};

biome get_biome(unsigned char temperature, unsigned char humidity) {

    if (temperature < 80) {
        if (humidity < 100) { return biomes[2]; }
        if (humidity < 160) { return biomes[5]; }
        return biomes[6];
    }

    if (temperature >= 80 && temperature <= 120) { return biomes[3]; }
    if (temperature >= 121 && temperature <= 130) { return biomes[1]; }

    if (temperature < 180) {
        if (humidity < 100) { return biomes[7]; }
        if (humidity < 200) { return biomes[9]; }
        return biomes[4];
    }

    if (temperature <= 255) {
        if (humidity < 160) { return biomes[8]; }
        return biomes[10];
    }

    return biomes[0];
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
    float amp = 1.2;

    for (int i = 0; i < 12; i++) {
        val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
        freq *= 1.5;
        amp *= 0.55f;
    }

    val *= 1.2;

    if (val > 1.0f) val = 1.0f;
    else if (val < -1.0f) val = -1.0f;

    int value = (val + 1) * 0.5f * 255;
    return value;
}

Texture2D GenerateMap(int** map) {
    Image image = GenImageColor(MAP_WIDTH, MAP_HEIGHT, WHITE);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            unsigned char temperature = perlin_generate(x + offset_x, y + offset_y);
            unsigned char humidity = perlin_generate(x + offset_z, y + offset_a);

            biome biome_current = get_biome(temperature, humidity);
            ImageDrawPixel(&image, x, y, biome_current.color);
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    return texture;
}

biome get_biome_current(Vector2 pos) {
    unsigned char temperature = perlin_generate((pos.x + offset_x) * map_multiplier, (pos.y + offset_y) * map_multiplier);
    unsigned char humidity = perlin_generate((pos.x + offset_z) * map_multiplier, (pos.y + offset_a) * map_multiplier);
    biome biome_current = get_biome(temperature, humidity);
    return biome_current;
}


#endif //PERLIN_H
