# C++ Pong – Multiball & Invert-Controls Edition

A modern C++17 remake of the classic Pong arcade game built with SDL3.  
Current power-ups:

* **Multiball** – splits the current ball into five.
* **Invert Controls** – reverses the paddle controls of the last player who touched the ball for 10 seconds.

---

## 1. Prerequisites

| Dependency | Version (or newer) | macOS (Homebrew) | Ubuntu / Debian | Windows (vcpkg) |
|------------|-------------------|-----------------|-----------------|-----------------|
| C++ compiler | C++17 capable (clang 15+, GCC 11+, MSVC 2022) | — | — | — |
| CMake | ≥ 3.20 | `brew install cmake` | `sudo apt install cmake` | `choco install cmake` or install manually |
| SDL3 | ≥ 3.0.0 | `brew install sdl3` | `sudo apt install libsdl3-dev`* | `vcpkg install sdl3` |
| SDL3_ttf | matching SDL3 | `brew install sdl3_ttf` | `sudo apt install libsdl3-ttf-dev`* | `vcpkg install sdl3-ttf` |

\*Until SDL3 packages land in all distributions you may need to build SDL3/SDL_ttf from source – follow the instructions at <https://github.com/libsdl-org/SDL>.

## 2. Building the game

### Quick way (bash)

```bash
./build.sh
```
The script creates a `build/` directory, runs CMake, and compiles the executable `CppPong`.

### Manual CMake

```bash
# At project root
cmake -S . -B build
cmake --build build --config Release
```

The binary will be at `build/CppPong` (or `build/Release/CppPong.exe` on Windows).

## 3. Running

```bash
# If you used build.sh you are already inside build/
./CppPong
```

## 4. Controls

| Action | Keys |
|--------|------|
| Move left paddle | **W** = up, **S** = down |
| Move right paddle | **↑** = up, **↓** = down |
| Quit game | **Esc** |
| Restart after game-over | **R** |

### Power-Ups

* **Multiball** – golden square. Touch it with the ball to spawn 5 balls.
* **Invert Controls** – magenta square with opposing arrows. The player who last hit the ball will have their **up / down** reversed for exactly 10 s.

## 5. Troubleshooting

* **Cannot find SDL3 headers** – verify that the SDL3 include and library paths are discoverable by your compiler. On Linux this usually means `pkg-config --cflags sdl3` returns a path.
* **Black window / no text** – ensure at least one of the system fonts listed in `src/Game.cpp` exists. Modify the font paths or copy a TTF file if necessary.

---
