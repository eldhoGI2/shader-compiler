# 🎨 Shader Program Compiler (OpenGL 2 + Custom DSL)

A Domain-Specific Language (DSL) and Source-to-Source Compiler (Transpiler) that converts intuitive `.my` graphics scripts into native **OpenGL 2.1 + SDL2** hardware-accelerated desktop applications and games.

---

## ⚡ Quick Start

### 1. Compile the Compiler
```bash
g++ -O2 compiler.cpp -o mycompiler
```

### 2. Compile Any Script
```bash
# Example 1: 🏓 Interactive Pong Game with AI & Collision
./mycompiler pong.my

# Example 2: 🌆 Cyberpunk theme with custom external GLSL shader & gradients
./mycompiler cyberpunk.my

# Example 3: 🌟 Shapes, colors & shader showcase
./mycompiler showcase.my

# Example 4: 📜 Original backwards-compatible demo
./mycompiler demo.my
```

### 3. Run Your Application
```bash
./final_game
```

---

## 🏓 Pong Game Mode (`pong.my`)
Writing an interactive game in our DSL is as simple as:
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

### 🎮 Game Controls
* **`[W]` / `[S]`** or **`[UP]` / `[DOWN]`**: Move your paddle up & down.
* **`[SPACE]`**: Live-cycle between active OpenGL 2 shaders in real time (`Grid` ➔ `Rainbow` ➔ `Pulse` ➔ `Vignette` ➔ `Custom` ➔ `None`).
* **`[R]`**: Reset game scores to `0 - 0` and reset the ball.
* **`[ESC]` or `[Q]`**: Close application cleanly.

---

## 📖 Complete Language Reference

### 1. Window & Environment
| Command | Arguments | Description |
|---|---|---|
| `window` | `<w> <h>` | Sets canvas dimensions (e.g. `window 800 600`) |
| `title` | `<string>` | Sets window title (e.g. `title "My OpenGL 2 Canvas"`) |
| `bg` / `background` | `<color>` | Sets background clear color (`bg #0f172a`, `bg navy`, `bg 20 20 40`) |

### 2. Interactive Game Mode (Pong)
| Command | Arguments | Description |
|---|---|---|
| `pong` | *none* | Activates 60 FPS interactive Pong game loop with AI and collision detection |
| `paddle` | `[<color>]` or `[<x> <y> <w> <h> <color>]` | Customizes player paddle size/color |
| `ai_paddle` | `[<color>]` or `[<x> <y> <w> <h> <color>]` | Customizes AI paddle size/color |
| `ball` | `[<color>]` or `[<x> <y> <radius> <color>]` | Customizes ball size/color |

### 3. Shaders (OpenGL 2 Programmable Hardware Pipeline)
| Command | Argument | Description |
|---|---|---|
| `shader` | `grid` | Cyberpunk / Synthwave animated neon matrix grid |
| `shader` | `rainbow` | Flowing dynamic chromatic color wave |
| `shader` | `pulse` | Pulsating glowing neon energy ring |
| `shader` | `vignette` | Cinematic darkened edge vignette |
| `shader` | `none` | Disables background shader |
| `custom_shader` | `<file.frag>` | Loads and runs any external GLSL fragment shader (e.g. `custom_shader plasma.frag`) |

### 4. Colors & Styles
* **Named Colors (35+ built-in presets)**:
  `red`, `green`, `blue`, `white`, `black`, `yellow`, `cyan`, `magenta`, `orange`, `purple`, `pink`, `gray`, `darkgray`, `lightgray`, `gold`, `silver`, `brown`, `lime`, `teal`, `navy`, `violet`, `indigo`, `coral`, `crimson`, `skyblue`, `turquoise`, `maroon`, `olive`, `mint`, `peach`, `lavender`, `amber`, `rose`.
* **Hex Color Codes**: `#ff007f`, `#00ffff`, `#1e293b`, `#f00`.
* **RGB / RGBA Numbers**: `color 255 128 0` (RGB) or `color 255 128 0 180` (RGBA with transparency).
* **Alpha Adjustment**: `alpha <0-255>` (e.g. `alpha 180`).
* **Stroke & Point Size**: `line_width <w>` and `point_size <s>`.

### 5. Gradients (OpenGL Vertex Interpolation)
* `gradient_rect <x> <y> <w> <h> <top_color> <bottom_color>`: Vertical color gradient rectangle.
* `gradient_h_rect <x> <y> <w> <h> <left_color> <right_color>`: Horizontal color gradient rectangle.
* `gradient_triangle <x1> <y1> <c1> <x2> <y2> <c2> <x3> <y3> <c3>`: Classic multi-color OpenGL triangle.

### 6. Transformations (OpenGL Matrix Operations)
* `rotate <angle_degrees>`: Rotates subsequent shapes around the active origin.
* `translate <dx> <dy>`: Moves coordinate origin in 2D space.
* `scale <sx> <sy>`: Scales subsequent shapes.
* `push` / `pop`: Saves (`glPushMatrix`) and restores (`glPopMatrix`) coordinate matrices.
* `reset_transform`: Resets transform back to identity (`glLoadIdentity`).

### 7. Geometric Primitives
#### Rectangles & Squares
* `rect <x> <y> <w> <h>` / `rectangle <x> <y> <w> <h>` (Solid filled)
* `rect_outline <x> <y> <w> <h>` / `stroke_rect <x> <y> <w> <h>` (Outline)
* `center_rect <w> <h>` / `center_rect_outline <w> <h>` (Centered)
* `square <x> <y> <size>` / `square_outline <x> <y> <size>`
* `center_square <size>` / `center_square_outline <size>`
* `rounded_rect <x> <y> <w> <h> <radius>` (Filled with rounded corners)
* `rounded_rect_outline <x> <y> <w> <h> <radius>` (Outline with rounded corners)

#### Circles & Ellipses
* `circle <cx> <cy> <radius>` (Outline circle)
* `fill_circle <cx> <cy> <radius>` (Solid circle)
* `center_circle <radius>` / `fill_center_circle <radius>`
* `ellipse <cx> <cy> <rx> <ry>` (Outline ellipse)
* `fill_ellipse <cx> <cy> <rx> <ry>` (Solid ellipse)

#### Triangles, Stars & Polygons
* `triangle <x1> <y1> <x2> <y2> <x3> <y3>` (Outline triangle)
* `fill_triangle <x1> <y1> <x2> <y2> <x3> <y3>` (Solid triangle)
* `quad <x1> <y1> ... <x4> <y4>` / `fill_quad <x1> <y1> ... <x4> <y4>`
* `polygon <cx> <cy> <radius> <sides>` (e.g. Hexagon: `polygon 400 300 60 6`)
* `fill_polygon <cx> <cy> <radius> <sides>`
* `star <cx> <cy> <outer_r> <inner_r> <points>` (Outline star)
* `fill_star <cx> <cy> <outer_r> <inner_r> <points>` (Solid star)

#### Lines & Points
* `line <x1> <y1> <x2> <y2>`
* `point <x> <y>` / `dot <x> <y>`

---

## 🏛️ System Architecture
1. **Frontend Transpiler (`compiler.cpp`)**: Tokenizes `.my` scripts and emits optimized C code (`generated_game.c`).
2. **Runtime Engine (`graphics_runtime.h`)**: Bridges SDL2 and OpenGL 2.1:
   * Sets up 2D coordinate projections (`glOrtho`).
   * Handles continuous keyboard polling via `SDL_GetKeyboardState`.
   * Calculates Axis-Aligned Bounding Box (AABB) collisions.
   * Compiles user geometry into high-performance OpenGL Display Lists.
   * Compiles and executes programmable GLSL 1.20 fragment shaders with uniforms `u_time` and `u_resolution`.
   * Features a built-in 7-segment digital arcade scoreboard.
   * Manages hardware double-buffering at 60 FPS.
3. **Build Toolchain**: Automatically calls `gcc generated_game.c -o final_game -lSDL2 -lGL -lm`.
