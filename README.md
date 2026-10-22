# 🎨 Shader Program Compiler (OpenGL 2 + 2D/3D Engine DSL)

A Domain-Specific Language (DSL) and Source-to-Source Compiler (Transpiler) that converts high-level `.my` scripts into native **OpenGL 2.1 + SDL2** hardware-accelerated desktop applications, 2D arcade games, and 3D first-person environments.

---

## ⚡ Quick Start

### 1. Compile the Compiler
```bash
g++ -O2 compiler.cpp -o mycompiler
```

### 2. Compile Any Script
```bash
# 🕹️ 1. 3D First-Person POV Room Explorer with Wall Collision
./mycompiler world3d.my

# 🏓 2. 2D Cyberpunk Pong with AI & Collision
./mycompiler pong.my

# 🌆 3. 2D Cyberpunk Theme with Custom GLSL Shader & Gradients
./mycompiler cyberpunk.my

# 🌟 4. Complete 2D Shapes & 35+ Colors Showcase
./mycompiler showcase.my

# 📜 5. Original Review 1 & 2 Prototype Baseline
./mycompiler demo.my
```

### 3. Run Your Application
```bash
./final_game
```

---

## 🕹️ 3D First-Person POV World (`world3d.my`)
Declare a fully navigable 3D environment in your DSL in just a few lines:
```text
title "3D First-Person POV World (OpenGL 2 Engine)"
window 900 700
bg #060614

# Activate 3D First-Person Mode!
mode_3d

# Floor & Perimeter Walls
floor 40 #161a2e #0e1224
walls 40 5 #0f3460

# 3D Objects with Axis-Aligned Bounding Box Collision
cube 0 1 -10 2 gold
cube -8 1 -6 2 cyan
cube 8 1 -6 2 magenta
cube -6 1 6 2 lime
cube 6 1 6 2 crimson

# Monolith Pillars
pillar -12 0 2 5 #e94560
pillar 12 0 2 5 #00adb5
```

### 🎮 3D Controls
* **`[W]` / `[S]`**: Walk forward and backward in the direction you are facing.
* **`[A]` / `[D]`**: Strafe left and right.
* **`[Left]` / `[Right]`** or **`[Q]` / `[E]`** or **Mouse**: Look around in 360° first-person view.
* **`[M]`**: Toggle mouse capture on/off.
* **`[SPACE]`**: Live-cycle through GPU shaders & ambient lighting.
* **`[ESC]`**: Exit back to desktop.

---

## 🏓 2D Pong Game Mode (`pong.my`)
```text
title "Cyberpunk Pong"
window 800 600
shader grid
bg #060614

pong
paddle cyan
ai_paddle magenta
ball gold
```

### 🎮 Pong Controls
* **`[W]` / `[S]`** or **`[UP]` / `[DOWN]`**: Move paddle up and down.
* **`[SPACE]`**: Live-cycle shaders.
* **`[R]`**: Reset score to `0 - 0` and reset ball.
* **`[ESC]`**: Quit.

---

## 📖 Complete Language Reference

### 1. Canvas & Environment
| Command | Arguments | Description |
|---|---|---|
| `window` | `<w> <h>` | Sets canvas dimensions (e.g. `window 900 700`) |
| `title` | `<string>` | Sets window title (e.g. `title "My 3D World"`) |
| `bg` / `background` | `<color>` | Sets background clear color (`bg #0f172a`, `bg navy`, `bg 20 20 40`) |

### 2. 3D First-Person Engine
| Command | Arguments | Description |
|---|---|---|
| `mode_3d` / `3d` | *none* | Activates 3D first-person perspective rendering with depth testing & collision |
| `floor` | `<size> [<color1>] [<color2>]` | Creates a 3D checkerboard floor grid |
| `walls` | `<size> <height> [<color>]` | Builds 4 surrounding enclosure walls with collision boundary |
| `cube` | `<x> <y> <z> <size> [<color>]` | Places a solid directional-shaded 3D cube with collision |
| `pillar` | `<x> <z> <width> <height> [<color>]` | Places a 3D architectural pillar |

### 3. 2D Pong Game Engine
| Command | Arguments | Description |
|---|---|---|
| `pong` | *none* | Activates interactive 2D Pong game loop with AI and scoreboard |
| `paddle` | `[<color>]` or `[<x> <y> <w> <h> <color>]` | Customizes player paddle size/color |
| `ai_paddle` | `[<color>]` or `[<x> <y> <w> <h> <color>]` | Customizes AI paddle size/color |
| `ball` | `[<color>]` or `[<x> <y> <radius> <color>]` | Customizes ball size/color |

### 4. Shaders & Effects
| Command | Argument | Description |
|---|---|---|
| `shader` | `grid`, `rainbow`, `pulse`, `vignette`, `none` | Built-in GLSL 1.20 hardware shaders |
| `custom_shader` | `<file.frag>` | Loads and runs any external GLSL fragment shader |

### 5. Colors, Gradients & Transformations
* **35+ Named Colors**: `red`, `green`, `blue`, `gold`, `silver`, `cyan`, `magenta`, `lime`, `teal`, `navy`, `coral`, `crimson`, `skyblue`, etc.
* **Hex Codes & RGBA**: `#ff007f`, `#00ffff`, `color 255 128 0 180`, `alpha 200`.
* **Gradients**: `gradient_rect`, `gradient_h_rect`, `gradient_triangle`.
* **Transformations**: `rotate`, `translate`, `scale`, `push`, `pop`, `reset_transform`.

---

## 🏛️ System Architecture
1. **Frontend Transpiler (`compiler.cpp`)**: Tokenizes `.my` scripts and generates structured, optimized C code (`generated_game.c`).
2. **Runtime Engine (`graphics_runtime.h`)**:
   * Bridges SDL2, OpenGL 2.1, and GLU.
   * Handles 2D orthogonal and 3D perspective projections (`gluPerspective`, `gluLookAt`).
   * Implements 3D camera mathematics, continuous keyboard polling, and AABB wall sliding collision.
   * Compiles user geometry into OpenGL display lists.
   * Manages GLSL programmable fragment shaders with `u_time` and `u_resolution`.
3. **Build Toolchain**: Automatically calls `gcc generated_game.c -o final_game -lSDL2 -lGL -lGLU -lm`.
