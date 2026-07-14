// #include "raylib.h"
// #include <cmath> // for sqrtf

// // Function to check if two circles overlap
// bool CheckCircleCollision(Vector2 center1, float radius1, Vector2 center2, float radius2) {
//     float dx = center1.x - center2.x;
//     float dy = center1.y - center2.y;
//     float distance = sqrtf(dx * dx + dy * dy);
//     return distance < (radius1 + radius2);
// }

// int main() {
//     const int screenWidth = 800;
//     const int screenHeight = 450;

//     InitWindow(screenWidth, screenHeight, "Raylib C++ - Barrier Circle Example");

//     // Player properties
//     Vector2 playerPos = { 100, 100 };
//     float playerRadius = 20.0f;
//     float playerSpeed = 200.0f;

//     // Barrier circle properties
//     Vector2 barrierCenter = { 400, 225 };
//     float barrierRadius = 100.0f;

//     SetTargetFPS(60);

//     while (!WindowShouldClose()) {
//         // Movement input
//         Vector2 oldPos = playerPos;
//         if (IsKeyDown(KEY_RIGHT)) playerPos.x += playerSpeed * GetFrameTime();
//         if (IsKeyDown(KEY_LEFT))  playerPos.x -= playerSpeed * GetFrameTime();
//         if (IsKeyDown(KEY_DOWN))  playerPos.y += playerSpeed * GetFrameTime();
//         if (IsKeyDown(KEY_UP))    playerPos.y -= playerSpeed * GetFrameTime();

//         // Collision check: if colliding with barrier, revert to old position
//         if (CheckCircleCollision(playerPos, playerRadius, barrierCenter, barrierRadius)) {
//             playerPos = oldPos;
//         }

//         // Draw
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         // Draw barrier
//         DrawCircleV(barrierCenter, barrierRadius, RED);

//         // Draw player
//         DrawCircleV(playerPos, playerRadius, BLUE);

//         DrawText("Arrow keys to move. Can't enter red circle.", 10, 10, 20, DARKGRAY);

//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
// #include "raylib.h"

// int main() {
//     // Initialize window
//     InitWindow(800, 600, "Hollow Circle Example");
//     SetTargetFPS(60);

//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         // Draw a hollow circle at (400, 300) with radius 100
//         DrawCircleLines(400, 300, 300, RED);

//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
// #include "raylib.h"

// int main() {
//     InitWindow(800, 600, "2D Donut Example");

//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         // Draw a ring (outer radius 100, inner radius 50)
//         DrawRing({400, 300}, 450, 500, 0, 360, 64, RED);

//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
#include "raylib.h"

int main() {
    InitWindow(800, 600, "Shrinking Donut Example");

    float innerRadius = 500.0f;
    float outerRadius = 1300.0f;
    float shrinkSpeed = 3.0f; // pixels per second
    bool shrinking = true;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        float delta = GetFrameTime();

        if (shrinking) {
            innerRadius -= shrinkSpeed * delta;
            outerRadius -= shrinkSpeed * delta;
            if (outerRadius <= 25) shrinking = false; // switch to growing
        } else {
            innerRadius += shrinkSpeed * delta;
            outerRadius += shrinkSpeed * delta;
            if (outerRadius >= 250) shrinking = true; // switch to shrinking
        }

        // Prevent invalid ring
        if (innerRadius < 0) innerRadius = 0;

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRing({400, 300}, innerRadius, outerRadius, 0, 360, 128, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

