# agario-CS
We are making Agar.io as a final project for our computer science camp.

## Gameplay

- **Player 1**: WASD
- **Player 2**: Arrow keys
- Eat small colored balls to grow larger.
- Bigger circles move slower.
- If one player is significantly bigger and overlaps the other, they absorb them.
- A red ring pulses (shrinks then expands) — touching it shrinks your circle over time. Shrink too much and you die.
- Game ends when one or both players die. The end screen shows whether a player was absorbed or eliminated by the ring.

## Requirements

- C++17 (or later)
- [raylib](https://www.raylib.com/) installed or fetched via CMake

## Build

**CMake (recommended):**
```bash
cmake -B build
cmake --build build
```

**Manual (Linux, g++):**
```bash
g++ main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

**macOS:**
```bash
g++ main.cpp -o game -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
```

**Windows (MinGW):**
```bash
g++ main.cpp -o game -lraylib -lopengl32 -lgdi32 -lwinmm
```

## How it works

**`Player`** — Represents one circle. Stores position, size, color, whether it's alive, and whether it died by being absorbed (as opposed to dying to the ring).
- `MoveToward(target)`: figures out the direction to the target, moves a limited distance that way without overshooting, then keeps the player inside the world bounds. Bigger circles move slower.

**`Ball`** — A simple pellet: just a position, size, and color, with no behavior of its own.

**`TryAbsorb(a, b)`** — Checks if one player can eat the other, and if so, does it. A player can only absorb the other if it's at least 10% bigger and close enough to overlap the other's center. The winner grows based on combined area (not just radius), and the loser is marked dead and "absorbed."

**`SpawnBall(...)`** — Tries to find a safe, empty spot for a new pellet by testing random positions, rejecting any that are too close to other balls or inside the ring hazard. Gives up after 1000 tries.

**`ApplyRingDamage(player, ...)`** — If a player is touching the ring, shrinks them over time. Shrink enough and they die — but this death is *not* marked as absorption, so the game can tell the two death types apart.

**Ball collisions** — Each frame, every ball is checked against both players by simple distance. On top of that, the game also samples pixel colors around each ball's edge (from a screenshot of the current frame) to detect if it's sitting on top of the ring or a player.

**Ring hazard** — Instead of shrinking one-way, the ring pulses: it shrinks to a minimum size, then grows back out, then repeats — keeping the hazard relevant throughout the match.

**Game over** — Once a player dies, the game checks *how* they died (absorbed vs. ring) to decide the end message and screen color, then freezes gameplay and shows "GAME OVER."

## Known limitations

- The pixel-sampling collision check captures the full screen every frame, which is expensive and can misfire if unrelated UI elements share the same colors (red/green/blue). A pure geometric collision check would be more efficient.
- Ring center is currently hardcoded rather than configurable.
