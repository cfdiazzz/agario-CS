/*#include "raylib.h"
#include <vector>
#include <cmath>

struct Ball { //creates ball, defines its variables
    Vector2 position;
    float radius;
    Color color;
};

// Check if a ball is too close to other balls
bool IsTooClose(const Vector2& newPos, const std::vector<Ball>& balls, float minDistance) { //IsTooClose is a function that checks if a new position is too close to existing balls
    for (const auto& ball : balls) {
        float dx = newPos.x - ball.position.x;
        float dy = newPos.y - ball.position.y;
        float distance = (float)sqrt(dx * dx + dy * dy); // Calculate the distance between the new position and the current ball
        if (distance < minDistance) {
            return true;
        }
    }
    return false;
}

// IsInsideRing is a function to check if a ball is overlapping with the red ring barrier
bool IsInsideRing(const Vector2& pos, float radius, float innerRadius, float outerRadius) {
    Vector2 ringCenter = { 400.0f, 300.0f }; // Center of your donut ring
    float dx = pos.x - ringCenter.x;
    float dy = pos.y - ringCenter.y;
    float distFromCenter = (float)sqrt(dx * dx + dy * dy);

    // Ball overlaps with the ring if its distance range intersects the ring's thickness
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
    
    int colorChoice = GetRandomValue(0, 5); //spawns random colored ball
    switch (colorChoice) {
        case 0: newBall.color = YELLOW; break;
        case 1: newBall.color = ORANGE; break;
        case 2: newBall.color = PURPLE; break;
        case 3: newBall.color = GOLD; break;
        case 4: newBall.color = PINK; break;
        case 5: newBall.color = VIOLET; break;
    }

    int attempts = 0;
    const int maxAttempts = 1000; // Increased to give more chances during tight spaces

    while (attempts < maxAttempts) {
        newBall.position.x = (float)GetRandomValue(30, 770);
        newBall.position.y = (float)GetRandomValue(30, 570);
        
        // PROACTIVE CHECKS: Must not be near another ball AND must not be touching the red ring
        if (!IsTooClose(newBall.position, existingBalls, 25.0f) && 
            !IsInsideRing(newBall.position, newBall.radius, innerRadius, outerRadius)) {
            return true; 
        }
        attempts++;
    }
    return false; 
}

int main() {
    InitWindow(800, 600, "Donut and Balls Collision Avoidance");
    float innerRadius = 500.0f;
    float outerRadius = 1300.0f;
    float shrinkSpeed = 10.0f;
    bool shrinking = true;
    
    std::vector<Ball> balls;
    
    // Spawn 100 initial balls safely outside the initial ring boundaries
    for (int i = 0; i < 100; i++) {
        Ball tempBall;
        if (SpawnBall(balls, tempBall, innerRadius, outerRadius)) {
            balls.push_back(tempBall);
        }
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update Ring
        float delta = GetFrameTime();
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
            Vector2 checkPoints[5] = {
                balls[i].position,
                { balls[i].position.x, balls[i].position.y - balls[i].radius },
                { balls[i].position.x, balls[i].position.y + balls[i].radius },
                { balls[i].position.x - balls[i].radius, balls[i].position.y },
                { balls[i].position.x + balls[i].radius, balls[i].position.y }
            };

            for (int p = 0; p < 5; p++) {
                int posX = (int)checkPoints[p].x;
                int posY = (int)checkPoints[p].y;
                if (posX >= 0 && posX < 800 && posY >= 0 && posY < 600) {
                    Color pixelColor = GetImageColor(screenImage, posX, posY);
                    if (ColorEquals(pixelColor, RED) || ColorEquals(pixelColor, GREEN) || ColorEquals(pixelColor, BLUE)) {
                        shouldRespawn = true;
                        break;
                    }
                }
            }

            if (shouldRespawn) {
                Ball newBall;
                // Passes innerRadius and outerRadius to verify it doesn't instantly die on respawn
                if (SpawnBall(balls, newBall, innerRadius, outerRadius)) {
                    balls[i] = newBall;
                } else {
                    balls.erase(balls.begin() + i);
                    i--; 
                }
            }
        }
        UnloadImage(screenImage);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRing({400, 300}, innerRadius, outerRadius, 0, 360, 128, RED);

        for (const auto& b : balls) {
            DrawCircleV(b.position, b.radius, b.color);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
*/