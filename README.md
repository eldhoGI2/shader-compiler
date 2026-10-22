# Shader Program Compiler (OpenGL 2 DSL)

A Domain-Specific Language (DSL) and Source-to-Source Compiler (Transpiler) that converts high-level `.my` graphics scripts into native **OpenGL 2 + SDL2** graphics applications.

---

## 🚀 Quick Start

### 1. Compile the Compiler
```bash
g++ -O2 compiler.cpp -o mycompiler
```

### 2. Compile a Script
```bash
./mycompiler showcase.my
```

### 3. Run Your Application
```bash
./final_game
```

---

## 🎨 Language Reference

### 1. Window & Canvas
| Command | Parameters | Description |
|---|---|---|
| `window` | `<width> <height>` | Sets the canvas width and height (e.g. `window 800 600`) |
| `title` | `<title_string>` | Sets window title (e.g. `title "My OpenGL App"`) |
| `bg` / `background` | `<color_name>` / `<hex>` / `<r g b>` | Sets background clear color (e.g. `bg #0f172a`, `bg navy`, `bg 20 20 40`) |

### 2. Shaders (OpenGL 2 Hardware GLSL Shaders)
| Command | Parameter | Description |
|---|---|---|
| `shader` | `grid` | Cyberpunk / Synthwave animated neon grid |
| `shader` | `rainbow` | Flowing dynamic chromatic color wave |
| `shader` | `pulse` | Pulsating glowing energy circle |
| `shader` | `vignette` | Cinematic darkened edge vignette |
| `shader` | `none` | Disables background shader |

### 3. Colors & Styles
* **Named Colors (35+ presets)**:
  `red`, `green`, `blue`, `white`, `black`, `yellow`, `cyan`, `magenta`, `orange`, `purple`, `pink`, `gray`, `darkgray`, `lightgray`, `gold`, `silver`, `brown`, `lime`, `teal`, `navy`, `violet`, `indigo`, `coral`, `crimson`, `skyblue`, `turquoise`, `maroon`, `olive`, `mint`, `peach`, `lavender`, `amber`, `rose`.
* **Hex Codes**:
  `#FF5733`, `#00FFCC`, `#1e293b`
* **RGB / RGBA Numbers**:
  `color 255 128 0` (RGB) or `color 255 128 0 180` (RGBA with transparency)
* **Alpha Adjustment**:
  `alpha <0-255>` (e.g. `alpha 180`)
* **Stroke & Point Size**:
  `line_width <width>` (e.g. `line_width 3`)
  `point_size <size>` (e.g. `point_size 8`)

### 4. Shapes & Primitives
#### Rectangles & Squares
* `rectangle <x> <y> <w> <h>` / `rect <x> <y> <w> <h>` (Filled)
* `rect_outline <x> <y> <w> <h>` / `stroke_rect <x> <y> <w> <h>` (Outline)
* `center_rect <w> <h>` (Auto-centered filled)
* `center_rect_outline <w> <h>` (Auto-centered outline)
* `square <x> <y> <size>` (Filled square)
* `square_outline <x> <y> <size>` (Outline square)
* `center_square <size>` / `center_square_outline <size>`
* `rounded_rect <x> <y> <w> <h> <radius>` (Filled with rounded corners)
* `rounded_rect_outline <x> <y> <w> <h> <radius>` (Outline with rounded corners)

#### Circles & Ellipses
* `circle <cx> <cy> <radius>` (Outline circle)
* `fill_circle <cx> <cy> <radius>` (Solid filled circle)
* `center_circle <radius>` / `fill_center_circle <radius>`
* `ellipse <cx> <cy> <rx> <ry>` (Outline ellipse)
* `fill_ellipse <cx> <cy> <rx> <ry>` (Solid filled ellipse)

#### Triangles, Stars & Polygons
* `triangle <x1> <y1> <x2> <y2> <x3> <y3>` (Outline triangle)
* `fill_triangle <x1> <y1> <x2> <y2> <x3> <y3>` (Solid triangle)
* `quad <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>` (Outline quad)
* `fill_quad <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>` (Solid quad)
* `polygon <cx> <cy> <radius> <sides>` (e.g. Hexagon: `polygon 400 300 60 6`)
* `fill_polygon <cx> <cy> <radius> <sides>` (Solid regular polygon)
* `star <cx> <cy> <outer_r> <inner_r> <points>` (Outline star)
* `fill_star <cx> <cy> <outer_r> <inner_r> <points>` (Solid star)

#### Lines & Points
* `line <x1> <y1> <x2> <y2>`
* `point <x> <y>` / `dot <x> <y>`

#### Comments
* `# This is a comment`
* `// This is also a comment`

---

## 🛠️ Architecture
1. **Frontend**: Custom Lexer & Parser in C++ (`compiler.cpp`).
2. **Intermediate Code**: Generated standard C source code (`generated_game.c`).
3. **Backend Graphics Engine**: Hardware-accelerated OpenGL 2.1 fixed & programmable pipeline wrapped in SDL2 (`graphics_runtime.h`).
4. **Linker flags**: `-lSDL2 -lGL -lm`.
