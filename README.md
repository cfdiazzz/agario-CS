# agario-CS
We are making Agar.io as a final project for our computer science camp.

## How To Play

Player 1 moves with **WASD**, Player 2 moves with the **arrow keys**. Both players start the same size and are just trying to survive and grow.

Rolling over the small colored balls scattered around the map makes you bigger. Size comes with a tradeoff though — the bigger you get, the slower you move, so growing too aggressively can leave you an easy, sluggish target.

If you're at least 10% bigger than the other player and you get close enough to overlap them, you absorb them and take on their size. It's not enough to just be bigger — you need to actually catch them.

The red ring in the middle isn't static. It shrinks down, then expands back out, over and over. If your circle is touching it, you shrink continuously for as long as you're in contact, and if you shrink down too far, you die. Getting eliminated by the ring is treated differently from being absorbed — the end-of-game screen will tell you which one happened.

The game ends the moment one or both players are eliminated, and the screen fills with either the winner's color (if it was an absorption) or red (if it was the ring), along with a short message explaining what happened.

## Installing and Running It

This is a plain C++ project with one real dependency: **raylib**. There's no package manager built into the project itself, so getting it running comes down to two steps — getting raylib onto your machine, and then compiling `main.cpp` against it.

### Option 1: Let CMake Handle Raylib For You (easiest)

If you have CMake installed, you don't need to install raylib yourself at all. The included `CMakeLists.txt` is set up to download and build raylib automatically the first time you compile:

```bash
cmake -B build
cmake --build build
```

The first run will take a little longer since it's fetching and building raylib from source in the background. After that, rebuilds are fast. Your executable will show up inside the `build` folder.

This approach is the least error-prone because it sidesteps version mismatches between your system's raylib and what the code expects, and it works the same way on Linux, macOS, and Windows as long as CMake and a C++ compiler are available.

### Option 2: Install Raylib Yourself and Compile It Manually

If you'd rather install raylib system-wide (or already have it), you can skip CMake and call your compiler directly. The catch is that raylib itself depends on a few system libraries for graphics, windowing, and input, so those need to be linked in too — which library depends on your OS.

**Linux:**
First install raylib through your package manager, or build it from source following raylib's own instructions. Once it's installed:
```bash
g++ main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```
The extra flags after `-lraylib` are raylib's own dependencies — OpenGL for rendering, and threading/X11 libraries for windowing and input on Linux.

**macOS:**
Install raylib with Homebrew (`brew install raylib`) or build from source, then compile with:
```bash
g++ main.cpp -o game -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
```
macOS uses system frameworks instead of standalone libraries, which is why the flags look different from the Linux command.

**Windows (MinGW):**
Download a prebuilt raylib release for MinGW (or build it yourself), then compile with:
```bash
g++ main.cpp -o game -lraylib -lopengl32 -lgdi32 -lwinmm
```
If you're using Visual Studio instead of MinGW, raylib also ships Visual Studio project templates — in that case you'd link against `raylib.lib` through the IDE's project settings rather than a command-line flag.

### Why Two Options Exist

The CMake path exists mainly to avoid the most common source of frustration with raylib projects: linking against the wrong version, or forgetting one of the platform-specific system libraries it depends on. If you're just trying to get the game running, start there. The manual route is more useful once you're comfortable with the toolchain and want more control over how raylib is built or where it lives on your system.

## How The Code Works

**`Player`** is the class behind each circle. It tracks position, size, color, whether it's still alive, and — importantly — *how* it died, since the game distinguishes between being absorbed by the other player and being worn down by the ring. Its one real behavior is `MoveToward()`, which nudges the circle toward wherever the player is steering, without ever overshooting the target, and keeps it from wandering outside the playable world. Speed is tied to size, so the movement math itself is where the "bigger is slower" rule lives.

**`Ball`** is much simpler — just a position, a size, and a color. It has no behavior of its own; everything that happens to a ball (spawning it, checking if it's been eaten) is handled by outside functions.

**`TryAbsorb`** runs once per frame and checks whether either player is now big enough and close enough to absorb the other. The size threshold (10% bigger) stops absorptions from happening between two nearly-equal players, and the winner's new size is calculated from combined *area* rather than just adding radii, which keeps growth feeling proportional rather than exploding in size with each absorption.

**`SpawnBall`** is responsible for placing pellets somewhere legal — not overlapping another ball, and not inside the ring hazard. It does this by trying random positions and checking each one, giving up after a large number of attempts so it can never get stuck in an infinite search if the map is too crowded.

**`ApplyRingDamage`** handles the ring's threat: any player currently touching the ring loses size steadily for as long as contact continues, and dies outright if they shrink past a minimum threshold. Crucially, this kind of death is never marked as an absorption, which is what lets the end screen correctly report "eliminated by the ring" instead of crediting the other player.

**Ball collision detection** happens two ways at once each frame: a straightforward distance check against both players (whoever's close enough eats the ball), and a second, more unusual check that samples the actual rendered pixel colors around each ball's edges to see if it's currently sitting on top of the ring or a player. This second method is a shortcut that avoids writing separate geometric overlap logic for every hazard type, though it comes at the cost of capturing the whole screen every frame, which isn't free.

**The ring's pulse** — shrinking to a minimum size, then expanding back out again, on a loop — is what keeps the hazard dangerous throughout an entire match rather than just being a one-time closing circle that stops mattering once it's fully closed.

**Game-over handling** ties all of this together: once a player dies, the game looks at *why* they died to decide the message and the color that fills the screen, then freezes all gameplay so the result stays visible until the game is closed.
