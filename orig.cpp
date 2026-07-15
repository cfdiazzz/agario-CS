/*#include "raylib.h"
#include <vector>
#include <cmath>

struct Ball { //variables for the struct Ball function
    Vector2 position;
    float radius;
    Color color;
};

// Check if a new ball is too close to any existing balls
bool IsTooClose(const Vector2& newPos, const std::vector<Ball>& balls, float minDistance) {
    for (const auto& ball : balls) {
        float dx = newPos.x - ball.position.x;
        float dy = newPos.y - ball.position.y;
        float distance = std::sqrt(dx * dx + dy * dy); // pythagorean theorum to calculate distance between two points
        float distance = std::sqrt(dx * dx + dy * dy); // pythagorean theorum to calculate distance between two points
        if (distance < minDistance) { //if the distance the ball will spawn is bigger than the minimum distance, then it will return true, meaning it is too close to another ball
            return true; // Too close!
        }
    }
    return false;
}

// Helper to check if two colors are identical
bool ColorEquals(Color c1, Color c2) {
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b); //
}

// Spawn a new valid ball (Strictly excludes RED, GREEN, and BLUE)
Ball SpawnBall(const std::vector<Ball>& existingBalls) {
    Ball b;
    b.radius = 8.0f;
    
    // Pick from colors that are NOT Red, Green, or Blue
    int colorChoice = GetRandomValue(0, 5);
    switch (colorChoice) {
        case 0: b.color = YELLOW; break;
        case 1: b.color = ORANGE; break;
        case 2: b.color = PURPLE; break;
        case 3: b.color = GOLD; break;
        case 4: b.color = PINK; break;
        case 5: b.color = VIOLET; break;
    }

    // Keep trying to find a position that is empty and not too close
    while (true) {
        b.position.x = (float)GetRandomValue(30, 770);
        b.position.y = (float)GetRandomValue(30, 570);
        
        if (!IsTooClose(b.position, existingBalls, 25.0f)) { //if too close, randomizes position and starts again
            break; // Valid spot found
        }
    }
    return b;
}

int main() {
    InitWindow(800, 600, "Donut and Balls Collision Avoidance");

    float innerRadius = 500.0f; //inner circle 
    float outerRadius = 1300.0f; //where the shape ends
    float shrinkSpeed = 10.0f; //how fast the donut shrinks/grows
    bool shrinking = true;

    std::vector<Ball> balls;
    
    // Spawn 100 initial balls
    for (int i = 0; i < 100; i++) {
        balls.push_back(SpawnBall(balls));
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

        // Update balls (Check color from screen memory)
        Image screenImage = LoadImageFromScreen(); 

        for (size_t i = 0; i < balls.size(); i++) {
            bool shouldRespawn = false;

            // Check 5 points: Center, Top, Bottom, Left, Right edge of the ball
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

                    // Trigger respawn if touching RED, GREEN, or BLUE
                    if (ColorEquals(pixelColor, RED) || 
                        ColorEquals(pixelColor, GREEN) || 
                        ColorEquals(pixelColor, BLUE)) {
                        shouldRespawn = true;
                        break; // Stop checking other points for this ball
                    }
                }
            }

            if (shouldRespawn) {
                balls[i] = SpawnBall(balls);
            }
        }
        
        UnloadImage(screenImage); 

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the shrinking/growing donut
        DrawRing({400, 300}, innerRadius, outerRadius, 0, 360, 128, RED);

        // Draw the balls
        for (const auto& b : balls) {
            DrawCircleV(b.position, b.radius, b.color);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
*/