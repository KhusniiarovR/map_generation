#include "raylib.h"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include "perlin.h"
#include <chrono>
#include <iostream>


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "my map");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();
    auto startTime = std::chrono::high_resolution_clock::now();
    unsigned int seed = time(nullptr);
    srand(seed);

    unsigned int frame = 0;

    offset_x = rand() % 10000;
    offset_y = rand() % 10000;
    offset_z = rand() % 10000;
    offset_a = rand() % 10000;

    Texture2D mapTexture = GenerateMap();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = endTime - startTime;
    std::cout << "Map loaded in: " << elapsed.count() << " seconds" << std::endl;

    Camera2D camera = { 0 };
    camera.offset = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    camera.target = {0, 0};
    camera.zoom = 5.0f;
    camera.rotation = 0.0f;

    Vector2 player_pos = {(float)MAP_WIDTH/2, (float)MAP_HEIGHT/2};

    biome current_biome = biomes[0];
    check_active_chunks(player_pos);

    while (!WindowShouldClose()) {
        frame++;

        if (IsKeyPressed(KEY_LEFT_SHIFT)) { player_speed *= 0.5f; }
        if (IsKeyPressed(KEY_LEFT_CONTROL)) { player_speed *= 2.0f; }

        if (IsKeyPressed(KEY_R)) { map_shader = map_shader_collection[1]; }
        if (IsKeyPressed(KEY_T)) { map_shader = map_shader_collection[2]; }
        if (IsKeyPressed(KEY_Y)) { map_shader = map_shader_collection[0]; }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player_pos.x += player_speed;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) player_pos.x -= player_speed;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) player_pos.y += player_speed;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) player_pos.y -= player_speed;

        if (player_pos.x < 0.1f) player_pos.x = 0.1f;
        if (player_pos.y < 0.1f) player_pos.y = 0.1f;
        if (player_pos.x > MAP_WIDTH - 0.1f) player_pos.x = MAP_WIDTH - 0.1;
        if (player_pos.y > MAP_HEIGHT - 0.1f) player_pos.y = MAP_HEIGHT - 0.1;

        camera.target = player_pos;

        if (IsKeyDown(KEY_Q)) camera.zoom += 0.05f;
        if (IsKeyDown(KEY_E)) camera.zoom -= 0.05f;
        if (camera.zoom <= 0.05f) camera.zoom = 0.05f;
        if (camera.zoom >= 7.0f) camera.zoom = 7.0f;

        check_active_chunks(player_pos);

        if (frame % 120 == 0) current_biome = get_biome(player_pos, mapTexture);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        DrawTextureRec(mapTexture, map_texture_rect, active_chunks_start, map_shader);
        DrawCircle(player_pos.x, player_pos.y, 3, LIGHTGRAY);
        DrawLineEx({0,0}, {0, MAP_HEIGHT}, 15, PURPLE);
        DrawLineEx({0,0}, {MAP_WIDTH, 0}, 15, PURPLE);
        DrawLineEx({MAP_WIDTH,0}, {MAP_WIDTH, MAP_HEIGHT}, 15, PURPLE);
        DrawLineEx({0,MAP_HEIGHT}, {MAP_WIDTH, MAP_HEIGHT}, 15, PURPLE);

        EndMode2D();

        std::string biome_name = "Current biome: " + current_biome.name;
        DrawText(biome_name.c_str(), 10, 10, 30, RED);

        std::string player_pos_current = "Current player position: " + std::to_string(static_cast<int>(player_pos.x)) + ", " + std::to_string(static_cast<int>(player_pos.y));
        DrawText(player_pos_current.c_str(), 10, 50, 30, RED);

        EndDrawing();
    }

    UnloadTexture(mapTexture);
    CloseWindow();

    return 0;
}

// river to map generator, Getimagepixel to get biome