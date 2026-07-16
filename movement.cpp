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

// === Ball / world logic ===
struct Ball {
    Vector2 position;
    float radius;
    Color color;
};

// Check if a ball is too close to other balls
bool IsTooClose(const Vector2& newPos, const std::vector<Ball>& balls, float minDistance) {
    for (const auto& ball : balls) {
        float dx = newPos.x - ball.position.x;
        float dy = newPos.y - ball.position.y;
        float distance = (float)sqrt(dx * dx + dy * dy);
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

// Check if a ball is overlapping with the red ring barrier
bool IsInsideRing(const Vector2& pos, float radius, float innerRadius, float outerRadius) {
    Vector2 ringCenter = { 400.0f, 300.0f }; // Center of the donut ring
    float dx = pos.x - ringCenter.x;
    float dy = pos.y - ringCenter.y;
    float distFromCenter = (float)sqrt(dx * dx + dy * dy);

    if ((distFromCenter + radius >= innerRadius) && (distFromCenter - radius <= outerRadius)) {
        return true;
    }
    return false;
}

bool ColorEquals(Color c1, Color c2) {
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

// Updated SpawnBall requiring the ring metrics to evaluate safety proactively
bool SpawnBall(const std::vector<Ball>& existingBalls, Ball& newBall, float innerRadius, float outerRadius) {
    newBall.radius = 8.0f;

    int colorChoice = GetRandomValue(0, 5);
    switch (colorChoice) {
        case 0: newBall.color = YELLOW; break;
        case 1: newBall.color = ORANGE; break;
        case 2: newBall.color = PURPLE; break;
        case 3: newBall.color = GOLD; break;
        case 4: newBall.color = PINK; break;
        case 5: newBall.color = VIOLET; break;
    }

    int attempts = 0;
    const int maxAttempts = 1000;

    while (attempts < maxAttempts) {
        newBall.position.x = (float)GetRandomValue(30, 770);
        newBall.position.y = (float)GetRandomValue(30, 570);

        if (!IsTooClose(newBall.position, existingBalls, 25.0f) &&
            !IsInsideRing(newBall.position, newBall.radius, innerRadius, outerRadius)) {
            return true;
        }
        attempts++;
    }
    return false;
}

int main() {
    // Create the game window (shared by players and world)
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Coop Circles - Raylib (C++)");
    SetTargetFPS(60); // cap the game loop at 60 frames per second

    // Player 1: controlled with WASD keys
    Player p1({ 300.0f, 300.0f }, 25, GREEN);

    // Player 2: controlled with arrow keys, starts slightly off to the side for coop
    Player p2({ 400.0f, 300.0f }, 25, BLUE);

    // Ring/donut state
    float innerRadius = 500.0f;
    float outerRadius = 1300.0f;
    float shrinkSpeed = 10.0f;
    bool shrinking = true;

    // Balls
    std::vector<Ball> balls;
    for (int i = 0; i < 100; i++) {
        Ball tempBall;
        if (SpawnBall(balls, tempBall, innerRadius, outerRadius)) {
            balls.push_back(tempBall);
        }
    }

    // === Main loop ===
    while (!WindowShouldClose()) { // runs until the window is closed (e.g. Esc or X button)

        float delta = GetFrameTime();

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

        // Update Ring
        if (shrinking) {
            innerRadius -= shrinkSpeed * delta;
            outerRadius -= shrinkSpeed * delta;
            if (outerRadius <= 25) shrinking = false;
        } else {
            innerRadius += shrinkSpeed * delta;
            outerRadius += shrinkSpeed * delta;
            if (outerRadius >= 250) shrinking = true;
        }
        if (innerRadius < 0) innerRadius = 0;

        // Update balls (Check color from screen memory for dynamic collisions)
        Image screenImage = LoadImageFromScreen();
        for (size_t i = 0; i < balls.size(); i++) {
            bool shouldRespawn = false;

            // Check if a player is eating this pellet (grows the player, removes the pellet)
            const float pelletGrowthAmount = 2.0f; // flat radius increase per pellet eaten (linear growth)

            if (p1.alive) {
                float dx = balls[i].position.x - p1.pos.x;
                float dy = balls[i].position.y - p1.pos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < p1.radius) {
                    p1.radius += pelletGrowthAmount;
                    shouldRespawn = true;
                }
            }
            if (!shouldRespawn && p2.alive) {
                float dx = balls[i].position.x - p2.pos.x;
                float dy = balls[i].position.y - p2.pos.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < p2.radius) {
                    p2.radius += pelletGrowthAmount;
                    shouldRespawn = true;
                }
            }

            Vector2 checkPoints[5] = {
                balls[i].position,
                { balls[i].position.x, balls[i].position.y - balls[i].radius },
                { balls[i].position.x, balls[i].position.y + balls[i].radius },
                { balls[i].position.x - balls[i].radius, balls[i].position.y },
                { balls[i].position.x + balls[i].radius, balls[i].position.y }
            };

            if (!shouldRespawn) {
                for (int p = 0; p < 5; p++) {
                    int posX = (int)checkPoints[p].x;
                    int posY = (int)checkPoints[p].y;
                    if (posX >= 0 && posX < screenWidth && posY >= 0 && posY < screenHeight) {
                        Color pixelColor = GetImageColor(screenImage, posX, posY);
                        if (ColorEquals(pixelColor, RED) || ColorEquals(pixelColor, GREEN) || ColorEquals(pixelColor, BLUE)) {
                            shouldRespawn = true;
                            break;
                        }
                    }
                }
            }

            if (shouldRespawn) {
                Ball newBall;
                if (SpawnBall(balls, newBall, innerRadius, outerRadius)) {
                    balls[i] = newBall;
                } else {
                    balls.erase(balls.begin() + i);
                    i--;
                }
            }
        }
        UnloadImage(screenImage);

        // === Draw ===
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRing({400, 300}, innerRadius, outerRadius, 0, 360, 128, RED);

        for (const auto& b : balls) {
            DrawCircleV(b.position, b.radius, b.color);
        }

        if (p1.alive) DrawCircleV(p1.pos, p1.radius, p1.color);
        if (p2.alive) DrawCircleV(p2.pos, p2.radius, p2.color);

        EndDrawing();
    }

    CloseWindow(); // clean up and close the game window
    return 0;
}