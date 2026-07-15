#include <iostream>
#include "raylib.h"

int main() {
    // 1. Open a blank window (Width, Height, Title)
    InitWindow(800, 450, "Raylib Sync Test");
    SetTargetFPS(60);

    // 2. Main game loop
    while (!WindowShouldClose()) {
        // 3. Drawing phase
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw text on the screen: (Text, X, Y, FontSize, Color)
        DrawText("If you see this, Raylib is 100% working!", 140, 200, 24, LIGHTGRAY);
    
        EndDrawing();
    }

    // 4. Close the window and clear memory
    CloseWindow();
    return 0;
}