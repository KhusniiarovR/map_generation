#ifndef PERLIN_H
#define PERLIN_H

#include <string>
#include <vector>
#include <iostream>

int offset_x, offset_y, offset_z, offset_a;
const int MAP_WIDTH = 8000;
const int MAP_HEIGHT = 8000;
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1200;
float player_speed = 10.0f;
float temperature_scale = 12;
float humidity_scale = 10;
const int river_margin = 20;
const int river_size = 100;
const int chunk_size = 500;
int number_of_active_chunks[3] = {0, 0, 0};
int grid_size = MAP_WIDTH / chunk_size;
Vector2 active_chunks_start;
Rectangle map_texture_rect;
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
    //{"Purple", {140, 100, 140, 255}}, // 3
    {"Forest", {60, 140, 60, 255}}, // 4
    {"Taiga", {190, 190, 200, 255}}, // 5
    {"Gray biome", {100, 100, 100, 255}}, // 6
    {"Desert", {200, 200, 60, 255}}, // 7
    {"Volcano", {35, 30, 30, 255}}, // 8
    {"Valley", {120, 200, 120, 255}}, // 9
    {"Volcano fire side", {70, 25, 20, 255}}, // 10
    {"River", {0, 105, 148, 255}} // 11
};

void check_active_chunks(Vector2 pos) {
    number_of_active_chunks[0] = 0;
    number_of_active_chunks[1] = 0;
    number_of_active_chunks[2] = 0;

    int player_chunk = static_cast<int>(pos.x / chunk_size) + static_cast<int>(pos.y / chunk_size) * grid_size;

    int x = player_chunk % grid_size;
    int y = player_chunk / grid_size;

    bool flag = true;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (j + x > MAP_WIDTH / chunk_size - 1 || j + x < 0) continue;
            if (i + y > MAP_WIDTH / chunk_size - 1 || y + i < 0) continue;

            if (flag) {
                number_of_active_chunks[1] = (j + x) + (i + y) * grid_size;
                flag = false;
            } // какой чанк слева сверху первый

            number_of_active_chunks[0]++; // сколько чанков

            if (i == -1) number_of_active_chunks[2] = 1;
            if (i == 1 && number_of_active_chunks[2] == 1) number_of_active_chunks[2] = 2; // вертикальных чанков 3
        }
    }

    active_chunks_start = Vector2{static_cast<float>((number_of_active_chunks[1] % grid_size) * chunk_size), static_cast<float>((number_of_active_chunks[1] / grid_size) * chunk_size)};

    if (number_of_active_chunks[0] == 9) {
        map_texture_rect = Rectangle{active_chunks_start.x,active_chunks_start.y, 3*chunk_size, 3*chunk_size};
        return;
    }
    if (number_of_active_chunks[0] == 4) {
        map_texture_rect = Rectangle{active_chunks_start.x,active_chunks_start.y, 2*chunk_size, 2*chunk_size};
        return;
    }
    if (number_of_active_chunks[0] == 6 && number_of_active_chunks[2] == 2) {
        map_texture_rect = Rectangle{active_chunks_start.x,active_chunks_start.y, 2*chunk_size, 3*chunk_size};
        return;
    }
    map_texture_rect = Rectangle{active_chunks_start.x,active_chunks_start.y, 3*chunk_size, 2*chunk_size};
}

biome get_biome_type(Color color) {
    for (int i = 0; i < biomes.size(); i++) {
        if (color.r == biomes[i].color.r && color.g == biomes[i].color.g && color.b == biomes[i].color.b) return biomes[i];
    }
}

biome get_biome(Vector2 pos, Texture2D texture) {
    Image image = LoadImageFromTexture(texture);
    Color color = GetImageColor(image,pos.x, pos.y);
    UnloadImage(image);
    return get_biome_type(color);
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
    Image image_t = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, offset_x, offset_y, temperature_scale);
    Image image_h = GenImagePerlinNoise(MAP_WIDTH, MAP_HEIGHT, offset_a, offset_z, humidity_scale);

    Image image_comb = GenImageColor(MAP_WIDTH, MAP_HEIGHT, BLACK);

    Color *pixels_t = LoadImageColors(image_t);
    Color *pixels_h = LoadImageColors(image_h);
    Color *pixels_comb = LoadImageColors(image_comb);

    for (int i = 0; i < MAP_HEIGHT * MAP_WIDTH; i++) {
        pixels_comb[i] = get_biome_color(pixels_t[i].r, pixels_h[i].r);
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


#endif //PERLIN_H
