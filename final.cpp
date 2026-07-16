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
    bool alive = true; // false once absorbed by another player or shrank to death
    bool wasAbsorbed = false; // <--- FIX: Track HOW the player died

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
        b.wasAbsorbed = true; // <--- FIX: Mark that Player B was absorbed
    } else if (b.radius > a.radius * 1.1f && dist < b.radius) {
        float newArea = (PI * b.radius * b.radius) + (PI * a.radius * a.radius);
        b.radius = sqrtf(newArea / PI);
        a.alive = false;
        a.wasAbsorbed = true; // <--- FIX: Mark that Player A was absorbed
    }
}

// === Ball / world logic / variables ===
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
    const int maxAttempts = 1000; //ball can only attempt to spawn this many times

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

void ApplyRingDamage(Player &player, float innerRadius, float outerRadius, float deltaTime)
{
    if (!player.alive) return;
    Vector2 ringCenter = {400.0f, 300.0f};
    float dx = player.pos.x - ringCenter.x;
    float dy = player.pos.y - ringCenter.y; 
    float distance = sqrtf(dx * dx + dy * dy); // Distance from player to ring center

    bool touchingRing =
        (distance + player.radius >= innerRadius) && 
        (distance - player.radius <= outerRadius);

    if (touchingRing)
    {
        player.radius -= 5.0f * deltaTime;   // Shrink 5 pixels per second

        if (player.radius <= 8.0f)
        {
            player.alive = false; // Player dies, but wasAbsorbed remains FALSE
        }
    }
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Coop Circles - Raylib (C++)");
    SetTargetFPS(60); 

    float maxRadiusNeeded = sqrtf((screenWidth * screenWidth) + (screenHeight * screenHeight));
    bool gameOver = false;
    Color screenColor = BLACK; //default color so variable is never undefined
    const char* gameOverSubText = ""; //variable for the game over subtitle

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
    while (!WindowShouldClose()) { 

        float delta = GetFrameTime();

        if (!gameOver) {
            // Player 1 movement
            if (p1.alive) {
                float dx1 = (IsKeyDown(KEY_D) ? 1 : 0) - (IsKeyDown(KEY_A) ? 1 : 0); //right: 1-0=1, left: 0-1=-1
                float dy1 = (IsKeyDown(KEY_S) ? 1 : 0) - (IsKeyDown(KEY_W) ? 1 : 0); 
                if (dx1 != 0 || dy1 != 0) {
                    Vector2 target = { p1.pos.x + dx1 * 50, p1.pos.y + dy1 * 50 }; 
                    p1.MoveToward(target);
                }
            }

            // Player 2 movement
            if (p2.alive) { 
                float dx2 = (IsKeyDown(KEY_RIGHT) ? 1 : 0) - (IsKeyDown(KEY_LEFT) ? 1 : 0);
                float dy2 = (IsKeyDown(KEY_DOWN) ? 1 : 0) - (IsKeyDown(KEY_UP) ? 1 : 0);
                if (dx2 != 0 || dy2 != 0) {
                    Vector2 target = { p2.pos.x + dx2 * 50, p2.pos.y + dy2 * 50 };
                    p2.MoveToward(target);
                }
            }

            // Check for absorption between the two players
            TryAbsorb(p1, p2);

            // Apply ring damage
            ApplyRingDamage(p1, innerRadius, outerRadius, delta); 
            ApplyRingDamage(p2, innerRadius, outerRadius, delta); 

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

            // === FIX: Check Game Over Conditions based on how they died ===
            if (!p1.alive && !p2.alive) {
                screenColor = RED; 
                gameOverSubText = "Both Players Eliminated!";
                gameOver = true;
            }
            else if (!p2.alive && p1.alive) {
                if (p2.wasAbsorbed) {
                    screenColor = p1.color;
                    gameOverSubText = "Player 1 Absorbed Player 2!";
                } else {
                    screenColor = RED;
                    gameOverSubText = "Player 2 was eliminated by the Ring!";
                }
                gameOver = true;
            }
            else if (!p1.alive && p2.alive) {
                if (p1.wasAbsorbed) {
                    screenColor = p2.color;
                    gameOverSubText = "Player 2 Absorbed Player 1!";
                } else {
                    screenColor = RED;
                    gameOverSubText = "Player 1 was eliminated by the Ring!";
                }
                gameOver = true;
            }

            // Update balls
            // 1. CAPTURE SCREEN STATE
// Take a screenshot of the current frame to use for pixel-color collision detection later
Image screenImage = LoadImageFromScreen();

// 2. ITERATE THROUGH ALL ACTIVE BALLS/PELLETS
for (size_t i = 0; i < balls.size(); i++) {
    bool shouldRespawn = false;            // Tracks if the current ball needs to be reset/replaced
    const float pelletGrowthAmount = 2.0f; // The amount a player grows when consuming a ball

    // 3. PLAYER 1 COLLISION CHECK
    if (p1.alive) {
        // Calculate the straight-line distance between Player 1 and the current ball
        float dx = balls[i].position.x - p1.pos.x;
        float dy = balls[i].position.y - p1.pos.y;
        float dist = sqrtf(dx * dx + dy * dy); // Pythagorean theorem (A² + B² = C²)

        // If distance is less than Player 1's radius, Player 1 is touching/overlapping the ball
        if (dist < p1.radius) {
            p1.radius += pelletGrowthAmount;   // Increase Player 1's size
            shouldRespawn = true;              // Mark this ball to be respawned
        }
    }

    // 4. PLAYER 2 COLLISION CHECK
    // Only check Player 2 if Player 1 didn't already consume this specific ball
    if (!shouldRespawn && p2.alive) {
        // Calculate the straight-line distance between Player 2 and the current ball
        float dx = balls[i].position.x - p2.pos.x;
        float dy = balls[i].position.y - p2.pos.y;
        float dist = sqrtf(dx * dx + dy * dy); // Pythagorean theorem

        // If distance is less than Player 2's radius, Player 2 is touching/overlapping the ball
        if (dist < p2.radius) {
            p2.radius += pelletGrowthAmount;   // Increase Player 2's size
            shouldRespawn = true;              // Mark this ball to be respawned
        }
    }
                // 1. DEFINE DETECTOR POINTS
                // Create a cross-pattern of 5 points to check the ball's boundaries
                Vector2 checkPoints[5] = {
                    balls[i].position,                                             // Center point
                    { balls[i].position.x, balls[i].position.y - balls[i].radius }, // Top edge
                    { balls[i].position.x, balls[i].position.y + balls[i].radius }, // Bottom edge
                    { balls[i].position.x - balls[i].radius, balls[i].position.y }, // Left edge
                    { balls[i].position.x + balls[i].radius, balls[i].position.y }  // Right edge
                };

                // 2. PIXEL COLOR COLLISION DETECTION
                // Only run this if a player didn't already eat the ball
                if (!shouldRespawn) {
                    for (int p = 0; p < 5; p++) {
                        // Cast coordinates to integers for pixel array indexing
                        int posX = (int)checkPoints[p].x;
                        int posY = (int)checkPoints[p].y;
                        
                        // Safety Check: Ensure the points are actually inside the screen boundaries
                        if (posX >= 0 && posX < screenWidth && posY >= 0 && posY < screenHeight) {
                            // Extract the exact color of the pixel from the screenshot
                            Color pixelColor = GetImageColor(screenImage, posX, posY);
                            
                            // Trigger collision if the pixel hits a dangerous/obstacle color (RED, GREEN, or BLUE)
                            if (ColorEquals(pixelColor, RED) || ColorEquals(pixelColor, GREEN) || ColorEquals(pixelColor, BLUE)) {
                                shouldRespawn = true; // Mark the ball to be reset
                                break;                // Stop checking the other points for this ball
                            }
                        }
                    }
                }

                // 3. RESPOND TO COLLISION (RESPAWN OR DELETE)
                if (shouldRespawn) {
                    Ball newBall;
                    // Attempt to find a valid new random position for the ball
                    if (SpawnBall(balls, newBall, innerRadius, outerRadius)) {
                        balls[i] = newBall; // Successfully repositioned; overwrite old ball data
                    } else {
                        // If no space is left to spawn, remove the ball completely from the vector
                        balls.erase(balls.begin() + i);
                        i--; // Decrement index to prevent skipping the next item due to the shift
                    }
                }
            } // END OF FOR LOOP
            
            // 4. MEMORY CLEANUP
            // Free the screenshot memory to prevent severe memory leaks every frame
            UnloadImage(screenImage);
        }

        // 5. GAME OVER STATE HANDLING
        if (gameOver) {
            // Expand surviving players to maximum size to trigger a win/fill visual effect
            if (p1.alive) p1.radius = maxRadiusNeeded;
            if (p2.alive) p2.radius = maxRadiusNeeded;
        }

        // === Drawing block ===
        BeginDrawing();
        
        if (!gameOver) {
            ClearBackground(RAYWHITE);

            DrawRing({400, 300}, innerRadius, outerRadius, 0, 360, 128, RED);

            for (const auto& b : balls) {
                DrawCircleV(b.position, b.radius, b.color);
            }

            if (p1.alive) DrawCircleV(p1.pos, p1.radius, p1.color);
            if (p2.alive) DrawCircleV(p2.pos, p2.radius, p2.color);
        } 
        else {
            // Fill the screen instantly with the game over color
            ClearBackground(screenColor); 

            // Set up and center the main title
            int titleFontSize = 60;
            const char* titleText = "GAME OVER";
            int titleWidth = MeasureText(titleText, titleFontSize);
            int titleX = (screenWidth / 2) - (titleWidth / 2);
            int titleY = (screenHeight / 2) - (titleFontSize / 2);
            
            DrawText(titleText, titleX, titleY, titleFontSize, WHITE);

            // Set up and center your dynamic sub-text message
            int subFontSize = 24;
            int subWidth = MeasureText(gameOverSubText, subFontSize);
            int subX = (screenWidth / 2) - (subWidth / 2);
            int subY = titleY + titleFontSize + 20; 
            
            DrawText(gameOverSubText, subX, subY, subFontSize, LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow(); 
    return 0;
}