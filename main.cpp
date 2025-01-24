#include "raylib.h"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include "perlin.h"


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "my map");
    SetTargetFPS(60);
    //ToggleBorderlessWindowed();
    unsigned int seed = time(nullptr);
    srand(seed);

    a_mult = rand();
    b_mult = rand();
    c_mult = rand();

    offset_x = rand() % 10000;
    offset_y = rand() % 10000;
    offset_z = rand() % 10000;

    int** map = new int*[MAP_HEIGHT];
    for (int i = 0; i < MAP_HEIGHT; i++) {
        map[i] = new int[MAP_WIDTH];
    }

    Texture2D mapTexture = GenerateMap(map, seed);

    Camera2D camera = { 0 };
    camera.offset = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    camera.target = {0, 0};
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    Vector2 player_pos = {0, 0};

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT)) player_pos.x += 10 * player_speed;
        if (IsKeyDown(KEY_LEFT)) player_pos.x -= 10 * player_speed;
        if (IsKeyDown(KEY_DOWN)) player_pos.y += 10 * player_speed;
        if (IsKeyDown(KEY_UP)) player_pos.y -= 10 * player_speed;

        camera.target = player_pos;
        if (player_pos.x < 0) player_pos.x = 0;
        if (player_pos.y < 0) player_pos.y = 0;
        if (player_pos.x > MAP_WIDTH) player_pos.x = MAP_WIDTH;
        if (player_pos.y > MAP_HEIGHT) player_pos.y = MAP_HEIGHT;

        if (IsKeyDown(KEY_Q)) camera.zoom += 0.1f;
        if (IsKeyDown(KEY_E)) camera.zoom -= 0.1f;
        if (camera.zoom < 0.1f) camera.zoom = 0.1f;
        if (camera.zoom > 5.0f) camera.zoom = 5.0f;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
        DrawTextureEx(mapTexture, { -camera.offset.x, -camera.offset.y }, 0.0f, 1.0f, WHITE);

        EndMode2D();
        DrawText("Use arrow keys to move", 10, 10, 20, BLACK);
        EndDrawing();
    }

    for (int i = 0; i < MAP_HEIGHT; i++) {
        delete[] map[i];
    }
    delete[] map;

    UnloadTexture(mapTexture);
    CloseWindow();

    return 0;
}