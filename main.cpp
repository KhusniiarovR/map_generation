#include "raylib.h"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include "perlin.h"


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "my map");
    SetTargetFPS(60);
    ToggleBorderlessWindowed();
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

    Vector2 cameraOffset = { 0, 0 };

    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT)) cameraOffset.x += 10;
        if (IsKeyDown(KEY_LEFT)) cameraOffset.x -= 10;
        if (IsKeyDown(KEY_DOWN)) cameraOffset.y += 10;
        if (IsKeyDown(KEY_UP)) cameraOffset.y -= 10;


        if (cameraOffset.x < 0) cameraOffset.x = 0;
        if (cameraOffset.y < 0) cameraOffset.y = 0;
        if (cameraOffset.x > MAP_WIDTH - SCREEN_WIDTH) cameraOffset.x = MAP_WIDTH - SCREEN_WIDTH;
        if (cameraOffset.y > MAP_HEIGHT - SCREEN_HEIGHT) cameraOffset.y = MAP_HEIGHT - SCREEN_HEIGHT;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(mapTexture, { -cameraOffset.x, -cameraOffset.y }, 0.0f, 1.0f, WHITE);

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