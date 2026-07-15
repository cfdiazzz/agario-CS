// === Setup ===
#include "raylib.h"
#include "raymath.h"   // Clamp() is declared here, not in raylib.h
#include <cmath>
#include <vector>

#define WORLD_SIZE 3000  // size of the game world; players can't move outside this

// A class representing a controllable circle
class Player {
public:
    Vector2 pos;      // x, y position in the world
    float radius;     // size of the circle (affects speed too)
    Color color;      // fill color
    bool alive = true; // false once absorbed by another player

    Player(Vector2 startPos, float r, Color c)
        : pos(startPos), radius(r), color(c) {}

    // Moves this circle toward a target point, without exceeding its max speed
    void MoveToward(Vector2 target) {
        Vector2 delta = { target.x - pos.x, target.y - pos.y }; // distance to target
        float dist = sqrtf(delta.x * delta.x + delta.y * delta.y); // straight-line distance

        if (dist > 1.0f) { // only move if not already basically at the target
            float speed = fmaxf(1.0f, 4.0f - radius / 40.0f); // bigger circles move slower
            float move = fminf(speed, dist) / dist;           // scale so we don't overshoot
            pos.x += delta.x * move;
            pos.y += delta.y * move;
        }

        // Clamp position so the circle stays fully inside the world bounds
        pos.x = Clamp(pos.x, radius, WORLD_SIZE - radius);
        pos.y = Clamp(pos.y, radius, WORLD_SIZE - radius);
    }
};

// Checks if one player can absorb another (must be alive, bigger, and overlapping)
// The bigger circle needs to overlap the smaller one's center to "eat" it (simple agar.io rule)
void TryAbsorb(Player &a, Player &b) {
    if (!a.alive || !b.alive) return; // skip if either is already absorbed

    Vector2 delta = { b.pos.x - a.pos.x, b.pos.y - a.pos.y };
    float dist = sqrtf(delta.x * delta.x + delta.y * delta.y); // distance between centers

    // Bigger circle must be noticeably larger (e.g. 10%) and overlapping enough to absorb
    if (a.radius > b.radius * 1.1f && dist < a.radius) {
        // Grow the absorber based on the area of the absorbed circle (keeps size scaling realistic)
        float newArea = (PI * a.radius * a.radius) + (PI * b.radius * b.radius);
        a.radius = sqrtf(newArea / PI);
        b.alive = false; // absorbed circle is removed from play
    } else if (b.radius > a.radius * 1.1f && dist < b.radius) {
        float newArea = (PI * b.radius * b.radius) + (PI * a.radius * a.radius);
        b.radius = sqrtf(newArea / PI);
        a.alive = false;
    }
}

int main() {
    // Create the game window
    const int screenWidth = 600;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Coop Circles - Raylib (C++)");
    SetTargetFPS(60); // cap the game loop at 60 frames per second

    // Player 1: controlled with WASD keys
    Player p1({ WORLD_SIZE / 2.0f, WORLD_SIZE / 2.0f }, 25, GREEN);

    // Player 2: controlled with arrow keys, starts slightly off to the side for coop
    Player p2({ WORLD_SIZE / 2.0f + 100, WORLD_SIZE / 2.0f }, 30, BLUE);

    // === Main loop ===
    while (!WindowShouldClose()) { // runs until the window is closed (e.g. Esc or X button)

        // Player 1 movement: WASD sets a direction, then move p1 one step in that direction
        if (p1.alive) {
            float dx1 = (IsKeyDown(KEY_D) ? 1 : 0) - (IsKeyDown(KEY_A) ? 1 : 0); // +1 right, -1 left
            float dy1 = (IsKeyDown(KEY_S) ? 1 : 0) - (IsKeyDown(KEY_W) ? 1 : 0); // +1 down, -1 up
            if (dx1 != 0 || dy1 != 0) {
                Vector2 target = { p1.pos.x + dx1 * 50, p1.pos.y + dy1 * 50 }; // target 50px in that direction
                p1.MoveToward(target);
            }
        }

        // Player 2 movement: same idea, using arrow keys instead
        if (p2.alive) {
            float dx2 = (IsKeyDown(KEY_RIGHT) ? 1 : 0) - (IsKeyDown(KEY_LEFT) ? 1 : 0);
            float dy2 = (IsKeyDown(KEY_DOWN) ? 1 : 0) - (IsKeyDown(KEY_UP) ? 1 : 0);
            if (dx2 != 0 || dy2 != 0) {
                Vector2 target = { p2.pos.x + dx2 * 50, p2.pos.y + dy2 * 50 };
                p2.MoveToward(target);
            }
        }

        // Check for absorption between the two players each frame
        TryAbsorb(p1, p2);
    }

    CloseWindow(); // clean up and close the game window
    return 0;
}
