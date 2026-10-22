# 🎨 Shader Program Compiler (OpenGL 2 + Custom DSL)

A Domain-Specific Language (DSL) and Source-to-Source Compiler (Transpiler) that converts intuitive `.my` graphics scripts into native **OpenGL 2.1 + SDL2** hardware-accelerated desktop applications.

---

## ⚡ Quick Start

### 1. Compile the Compiler
```bash
g++ -O2 compiler.cpp -o mycompiler
```

### 2. Compile Any Script
```bash
# Example 1: Original backwards-compatible demo
./mycompiler demo.my

# Example 2: Complete shapes, colors & shader showcase
./mycompiler showcase.my

# Example 3: Cyberpunk theme with custom external GLSL shader & gradients
./mycompiler cyberpunk.my
```

### 3. Run Your Application
```bash
./final_game
```

---

## 🎮 Interactive Controls in Running Application
* **`[SPACE]`**: Live-cycle between active OpenGL 2 shaders in real time (`None` ➔ `Grid` ➔ `Rainbow` ➔ `Pulse` ➔ `Vignette` ➔ `Custom GLSL`).
* **`[ESC]` or `[Q]`**: Close application cleanly.

---

## 📖 Complete Language Reference

### 1. Window & Environment
| Command | Arguments | Description |
|---|---|---|
| `window` | `<w> <h>` | Sets canvas dimensions (e.g. `window 900 700`) |
| `title` | `<string>` | Sets window title (e.g. `title "My OpenGL 2 Canvas"`) |
| `bg` / `background` | `<color>` | Sets background clear color (`bg #0f172a`, `bg navy`, `bg 20 20 40`) |

### 2. Shaders (OpenGL 2 Programmable Hardware Pipeline)
| Command | Argument | Description |
|---|---|---|
| `shader` | `grid` | Cyberpunk / Synthwave animated neon matrix grid |
| `shader` | `rainbow` | Flowing dynamic chromatic color wave |
| `shader` | `pulse` | Pulsating glowing neon energy ring |
| `shader` | `vignette` | Cinematic darkened edge vignette |
| `shader` | `none` | Disables background shader |
| `custom_shader` | `<file.frag>` | Loads and runs any external GLSL fragment shader (e.g. `custom_shader plasma.frag`) |

### 3. Colors & Styles
* **Named Colors (35+ built-in presets)**:
  `red`, `green`, `blue`, `white`, `black`, `yellow`, `cyan`, `magenta`, `orange`, `purple`, `pink`, `gray`, `darkgray`, `lightgray`, `gold`, `silver`, `brown`, `lime`, `teal`, `navy`, `violet`, `indigo`, `coral`, `crimson`, `skyblue`, `turquoise`, `maroon`, `olive`, `mint`, `peach`, `lavender`, `amber`, `rose`.
* **Hex Color Codes**:
  `#ff007f`, `#00ffff`, `#1e293b`, `#f00`
* **RGB / RGBA Numbers**:
  `color 255 128 0` (RGB) or `color 255 128 0 180` (RGBA with transparency)
* **Alpha Adjustment**:
  `alpha <0-255>` (e.g. `alpha 180`)
* **Stroke & Point Size**:
  `line_width <w>` (e.g. `line_width 3`)
  `point_size <s>` (e.g. `point_size 8`)

### 4. Gradients (OpenGL Vertex Interpolation)
* `gradient_rect <x> <y> <w> <h> <top_color> <bottom_color>`: Vertical color gradient rectangle.
* `gradient_h_rect <x> <y> <w> <h> <left_color> <right_color>`: Horizontal color gradient rectangle.
* `gradient_triangle <x1> <y1> <c1> <x2> <y2> <c2> <x3> <y3> <c3>`: Classic multi-color OpenGL triangle.

### 5. Transformations (OpenGL Matrix Operations)
* `rotate <angle_degrees>`: Rotates subsequent shapes around the active origin.
* `translate <dx> <dy>`: Moves the origin in 2D space.
* `scale <sx> <sy>`: Scales subsequent shapes.
* `push`: Saves current coordinate matrix (`glPushMatrix`).
* `pop`: Restores coordinate matrix (`glPopMatrix`).
* `reset_transform`: Resets transform back to identity.

### 6. Geometric Primitives
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

#### Comments
* `# Single-line comment`
* `// Also a single-line comment`

---

## 🏛️ System Architecture
1. **Frontend Transpiler (`compiler.cpp`)**: Tokenizes `.my` scripts and emits optimized C code (`generated_game.c`).
2. **Runtime Engine (`graphics_runtime.h`)**: Bridges SDL2 and OpenGL 2.1:
   * Sets up 2D coordinate projections (`glOrtho`).
   * Compiles user geometry into high-performance OpenGL Display Lists.
   * Compiles and executes programmable GLSL 1.20 fragment shaders with uniforms `u_time` and `u_resolution`.
   * Manages hardware double-buffering at 60 FPS.
3. **Build Toolchain**: Automatically calls `gcc generated_game.c -o final_game -lSDL2 -lGL -lm`.
