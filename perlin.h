#ifndef PERLIN_H
#define PERLIN_H

#include <string>
#include <vector>


int offset_x, offset_y, offset_z, offset_a;
const int MAP_WIDTH = 4000;
const int MAP_HEIGHT = 4000;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
float player_speed = 20.0f;
const int river_margin = 20;
const int river_size = 100;
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
    //{"Ocean", {0, 105, 148, 255}}, // 3
    {"Purple", {140, 100, 140, 255}}, // 3
    {"Forest", {60, 140, 60, 255}}, // 4
    {"Taiga", {190, 190, 200, 255}}, // 5
    {"Gray biome", {100, 100, 100, 255}}, // 6
    {"Desert", {200, 200, 60, 255}}, // 7
    {"Volcano", {35, 30, 30, 255}}, // 8
    {"Valley", {120, 200, 120, 255}}, // 9
    {"Volcano fire side", {70, 25, 20, 255}}, // 10
    {"River", {0, 105, 148, 255}} // 11
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

Color get_biome_color(unsigned char temperature, unsigned char humidity) {

    if (temperature < 80) {
        if (humidity < 100) { return biomes[2].color; }
        if (humidity < 160) { return biomes[5].color; }
        return biomes[6].color;
    }

    if (temperature >= 80 && temperature <= 120) { return biomes[3].color; }
    if (temperature >= 121 && temperature <= 130) { return biomes[1].color; }

    if (temperature < 180) {
        if (humidity < 100) { return biomes[7].color; }
        if (humidity < 200) { return biomes[9].color; }
        return biomes[4].color;
    }

    if (temperature <= 255) {
        if (humidity < 160) { return biomes[8].color; }
        return biomes[10].color;
    }

    return biomes[0].color;
}

Texture2D GenerateMap() {
    Image image_t = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, offset_x, offset_y, 15.0f);
    Image image_h = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, offset_a, offset_z, 20.0f);

    Image image_comb = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLACK);

    Color *pixels_t = LoadImageColors(image_t);
    Color *pixels_h = LoadImageColors(image_h);
    Color *pixels_comb = LoadImageColors(image_comb);

    for (int i = 0; i < MAP_HEIGHT * MAP_WIDTH; i++) {
        pixels_comb[i].r = pixels_t[i].r;
        pixels_comb[i].g = pixels_h[i].r;
    }

    UnloadImage(image_t);
    UnloadImage(image_h);
    UnloadImageColors(pixels_t);
    UnloadImageColors(pixels_h);

    Texture2D texture = LoadTextureFromImage(image_comb);
    UpdateTexture(texture, pixels_comb);
    UnloadImageColors(pixels_comb);
    UnloadImage(image_comb);

    return texture;
}

// biome get_biome_current(Vector2 pos) {
//
//
//     biome biome_current = get_biome(temperature, humidity);
//
//     return biome_current;
// }


#endif //PERLIN_H
