# 🎨 Shader Program Compiler: 2D/3D Graphics & Game DSL Engine

A high-level **Domain-Specific Language (DSL)** and **Source-to-Source Compiler (Transpiler)** built in C++ that compiles declarative `.my` graphics and game scripts into native, hardware-accelerated **OpenGL 2.1 + SDL2** desktop applications.

---

## 📑 Table of Contents
1. [Project Overview & Core Mission](#-project-overview--core-mission)
2. [How the System Works (End-to-End Pipeline)](#-how-the-system-works-end-to-end-pipeline)
3. [Complete File-by-File Guide](#-complete-file-by-file-guide)
4. [Evolution: Initial Prototype vs. Current Engine](#-evolution-initial-prototype-vs-current-engine)
5. [The 5 Demo Applications (How to Launch & How They Work)](#-the-5-demo-applications-how-to-launch--how-they-work)
   * [1. Demo: Original Coursework Baseline (`demo.my`)](#1-demo-original-coursework-baseline-demomy)
   * [2. Showcase: Shapes & 35+ Colors (`showcase.my`)](#2-showcase-shapes--35-colors-showcasemy)
   * [3. Cyberpunk: Shaders, Gradients & Matrices (`cyberpunk.my`)](#3-cyberpunk-shaders-gradients--matrices-cyberpunkmy)
   * [4. Pong: Interactive 2D Game with AI & Scoreboard (`pong.my`)](#4-pong-interactive-2d-game-with-ai--scoreboard-pongmy)
   * [5. World3D: 3D First-Person POV Environment (`world3d.my`)](#5-world3d-3d-first-person-pov-environment-world3dmy)
6. [Complete DSL Language & Syntax Reference](#-complete-dsl-language--syntax-reference)
7. [Technical Deep Dive & Architecture](#-technical-deep-dive--architecture)
8. [University Review & Viva Presentation Guide](#-university-review--viva-presentation-guide)

---

## 🌟 Project Overview & Core Mission

### The Problem
Graphics programming in C and C++ is notoriously verbose, error-prone, and full of boilerplate. To simply create a window, clear a background, and draw a moving circle or a 3D cube, a developer typically must:
* Manually initialize system display servers (X11 / Wayland via SDL2 or GLFW).
* Manage raw OpenGL context lifecycles, pixel formats, and double buffering.
* Write boilerplate GLSL shader compilation, attachment, and linking pipelines.
* Handle manual matrix transformations (orthographic or perspective math) and depth buffers.
* Manage raw pointers, memory leaks, event loops, and OS message pumps.

### The Solution
The **Shader Program Compiler** abstracts all low-level graphics plumbing behind a human-readable, declarative Domain-Specific Language (DSL). By writing scripts with a `.my` extension, users can render complex 2D vector art, compile GPU fragment shaders, play interactive arcade games with collision physics, and explore 3D worlds—**all with zero boilerplate, zero manual memory management, and zero pointers.**

---

## ⚙️ How the System Works (End-to-End Pipeline)

The system is a **4-stage source-to-source compiler (transpiler) and native toolchain**:

```
[ User DSL Script (.my) ]
           │
           ▼
[ 1. Lexical Analysis (compiler.cpp) ]
     • Strips whitespace & comments (#, //)
     • Tokenizes keywords, hex colors, and numbers
           │
           ▼
[ 2. Parsing & Code Generation (compiler.cpp) ]
     • Maps DSL tokens to C runtime APIs
     • Emits structured C source: generated_game.c
           │
           ▼
[ 3. Native Compilation (GCC Toolchain) ]
     • Links graphics_runtime.h
     • gcc generated_game.c -o final_game -lSDL2 -lGL -lGLU -lm
           │
           ▼
[ 4. Execution (final_game) ]
     • Hardware OpenGL 2.1 Compatibility Context
     • 60 FPS event loop with double buffering
     • GPU GLSL shaders + Display Lists + 3D Depth Buffer
```

1. **User writes a `.my` script**: A human-readable script defining window dimensions, background shaders, 2D shapes, 3D cubes, or interactive game modes.
2. **Lexical Analysis (`lexer`)**: `compiler.cpp` reads the raw text, ignores comments, parses hex codes (`#ff007f`) and strings, and tags words with strongly-typed tokens (`TOKEN_WINDOW`, `TOKEN_CUBE`, `TOKEN_PONG`, etc.).
3. **C Code Generation (`parse_and_generate`)**: The compiler emits an intermediate standard C file ([`generated_game.c`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/generated_game.c)). It inserts `#include "graphics_runtime.h"`, creates `int main()`, maps user parameters to C function calls, and selects the appropriate execution engine (`run_3d_game()`, `run_pong_game()`, or `render_and_wait()`).
4. **GCC Build Automation**: The compiler calls `gcc generated_game.c -o final_game -lSDL2 -lGL -lGLU -lm` via a system command.
5. **Hardware Execution**: The generated binary (`final_game`) executes natively on your Linux desktop with full GPU hardware acceleration at 60 FPS.

---

## 📂 Complete File-by-File Guide

```
shadercompailer/
├── compiler.cpp          # ⚙️ Core Transpiler (Lexer, Parser, Code Generator, GCC Build)
├── graphics_runtime.h    # 🎮 OpenGL 2.1, GLU & SDL2 Runtime Engine (2D, Pong, 3D)
├── mycompiler            # 🔨 Compiled executable of compiler.cpp
├── generated_game.c      # 📄 Intermediate C source code produced by mycompiler
├── final_game            # 🚀 Final compiled native graphics executable
├── demo.my               # 📜 Original Review 1 & 2 baseline script
├── showcase.my           # 🎨 2D Geometric primitives and 35+ named colors showcase
├── cyberpunk.my          # 🌆 2D Cyberpunk demo (custom GLSL shader, gradients, matrix rotation)
├── plasma.frag           # 🌀 Raw GPU GLSL 1.20 fragment shader source file
├── pong.my               # 🏓 2D Interactive Pong game script with AI opponent
├── world3d.my            # 🕹️ 3D First-Person POV navigable world script with collision
├── dummy.c / dummy       # 🧪 Early standalone SDL2 prototype before compiler creation
├── README.md             # 📚 Comprehensive project documentation
├── report.pdf / .odt     # 📑 Formal university coursework project report
└── Review1_...odp        # 📊 Review 1 presentation slides
```

### 1. [`compiler.cpp`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/compiler.cpp)
* **Language**: C++17.
* **Role**: The compiler frontend and build coordinator.
* **Key Components**:
  * `enum TokenType`: Defines 40+ grammatical token types.
  * `COLOR_TABLE`: Hash map defining 35+ human-readable English colors to RGBA values.
  * `parse_hex_color()`: Converts hex strings (`#fff`, `#ff007f`, `#1e293b80`) into integer RGBA channels.
  * `lexer()`: Scans the input script, handles whitespace, skips comments, and tokenizes code.
  * `parse_and_generate()`: Emits valid, structured C code into `generated_game.c`.
  * `main()`: Takes the CLI argument (`./mycompiler <file.my>`) and triggers the GCC compilation toolchain.

### 2. [`graphics_runtime.h`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/graphics_runtime.h)
* **Language**: C99 (Header-Only).
* **Role**: The graphics and game engine backend linked with every compiled program.
* **Key Modules**:
  * **Window & Context Management**: Creates the SDL2 window and sets up OpenGL 2.1 with double buffering and 24-bit depth testing.
  * **GLSL Shader Engine**: Compiles and links fragment shaders (`rainbow`, `grid`, `pulse`, `vignette`, or external files like `plasma.frag`), sending uniforms `u_time` and `u_resolution` every frame.
  * **2D Vector Renderer**: Compiles shapes into GPU Display Lists (`glGenLists`) for high performance.
  * **Interactive Pong Engine (`run_pong_game`)**: Continuous keyboard polling (`SDL_GetKeyboardState`), smart AI paddle tracking, circle-vs-AABB bounce math, and a built-in 7-segment LED scoreboard.
  * **3D First-Person Engine (`run_3d_game`)**: Configures perspective projection (`gluPerspective`), first-person camera (`gluLookAt`), AABB wall-sliding collision physics, directional-shaded 3D cubes, and a 2D crosshair HUD.

### 3. [`generated_game.c`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/generated_game.c)
* **Role**: The intermediate C file produced by `mycompiler`.
* **Details**: It translates the high-level `.my` script into pure C function calls. You can open and read this file at any time to verify the exact C code your script was translated into!

### 4. `mycompiler` & `final_game`
* **`mycompiler`**: The compiler CLI binary. Run `./mycompiler <file.my>` to compile any script.
* **`final_game`**: The output binary generated by GCC. Run `./final_game` to launch the window.

### 5. [`plasma.frag`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/plasma.frag)
* **Role**: A standalone GLSL 1.20 fragment shader.
* **Details**: Computes complex sine-wave plasma fractals in real-time on your GPU, loaded dynamically via `custom_shader plasma.frag`.

### 6. [`dummy.c`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/dummy.c) & `dummy`
* **Role**: The initial exploratory prototype written in raw C and SDL2 to test drawing lines, circles, and triangles before the compiler architecture was built.

### 7. Documentation & Slides
* **[`report.pdf`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/report.pdf) & `report.odt`**: Official Semester 3 academic report detailing project goals and roadmaps.
* **`Review1_Prototype_Compiler.odp`**: Project review presentation deck.

---

## 🚀 Evolution: Initial Prototype vs. Current Engine

| Capability | Initial Prototype (Review 1 & 2) | Current Engine (Completed System) |
|---|---|---|
| **Backend Renderer** | Software SDL2 2D Renderer (`SDL_Renderer`) | Hardware **OpenGL 2.1 + GLU** Compatibility Pipeline |
| **GPU Shaders** | ❌ None (CPU software rasterization) | ✅ **Full GLSL Fragment Shader Pipeline** (`grid`, `rainbow`, `pulse`, `vignette`, custom files) |
| **Color Support** | Only numeric RGB triples (`color 255 0 0`) | ✅ **35+ Named Colors**, Hex Codes (`#ff007f`), RGBA, and Alpha commands |
| **2D Geometry** | 5 basic shapes (rect, circle, triangle, line, center_rect) | ✅ **20+ Primitives** (rounded rects, stars, regular polygons/hexagons, quads, ellipses, gradients) |
| **Matrix Transforms** | ❌ None | ✅ `rotate`, `translate`, `scale`, `push`, `pop`, `reset_transform` |
| **Interactivity & Games**| ❌ Static single-frame drawings only | ✅ **Interactive 2D Pong** with continuous input, AI opponent, collision, and digital LED scoreboard |
| **3D Capabilities** | ❌ None | ✅ **3D First-Person POV World** with 3D perspective camera, depth buffer, floor grid, 3D cubes, and wall-sliding collision |
| **Code Comments** | ❌ Crashed or threw parse errors on comments | ✅ Full comment support (`#` and `//`) |

---

## 🎮 The 5 Demo Applications (How to Launch & How They Work)

### 1. Demo: Original Coursework Baseline ([`demo.my`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/demo.my))
* **Launch Command**:
  ```bash
  ./mycompiler demo.my && ./final_game
  ```
* **What It Is**: The original 14-line script presented in your university Review 1 & 2 coursework.
* **How It Works**:
  1. Creates an 800×600 window.
  2. Sets color to blue (`0 0 255`) and draws a centered rectangle (`600 400`).
  3. Sets color to red (`255 0 0`) and draws a circle (`400 300 100`).
  4. Sets color to green (`0 255 0`) and draws a triangle.
  5. Sets color to white (`255 255 255`) and draws two crossing lines forming an 'X'.
* **Why It Matters**: Proves that the upgraded OpenGL 2 compiler maintains **100% backward-compatibility** with your team's original code.

---

### 2. Showcase: Shapes & 35+ Colors ([`showcase.my`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/showcase.my))
* **Launch Command**:
  ```bash
  ./mycompiler showcase.my && ./final_game
  ```
* **What It Is**: A visual showcase of the expanded 2D vector drawing library and color engine.
* **How It Works**:
  * Activates an animated cyberpunk grid shader (`shader grid`) behind a dark slate canvas (`bg #0f172a`).
  * Draws translucent cards with rounded corners (`rounded_rect_outline`, `fill_rounded_rect`).
  * Renders a golden 5-point star (`fill_star`, `star`) using trigonometric alternating vertices.
  * Draws a regular hexagon (`fill_polygon`, `polygon` with 6 sides).
  * Draws filled and outlined ellipses (`fill_ellipse`, `ellipse`).
  * Features multi-point quads, stroke widths (`line_width 3`), and point dots (`point_size 8`, `point`).

---

### 3. Cyberpunk: Shaders, Gradients & Matrices ([`cyberpunk.my`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/cyberpunk.my))
* **Launch Command**:
  ```bash
  ./mycompiler cyberpunk.my && ./final_game
  ```
* **What It Is**: An advanced demonstration of GPU shaders, multi-stop color gradients, and OpenGL matrix transformations.
* **How It Works**:
  * Loads the external GLSL shader file via `custom_shader plasma.frag`.
  * Draws a vertical sky gradient (`gradient_rect 50 50 800 250 #00ffff #ff007f`).
  * Uses alpha blending to render a glowing synthwave sun with horizontal retro cutouts.
  * Draws a horizontal terrain gradient (`gradient_h_rect 50 390 800 240 #800080 #00ffff`) with converging perspective lines.
  * Renders the classic 3-color interpolated OpenGL triangle (`gradient_triangle`).
  * Uses matrix operations (`push`, `translate 700 200`, `rotate 45`, `pop`) to render rotated geometry in local coordinate space.

---

### 4. Pong: Interactive 2D Game with AI & Scoreboard ([`pong.my`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/pong.my))
* **Launch Command**:
  ```bash
  ./mycompiler pong.my && ./final_game
  ```
* **Controls**:
  * **`W` / `S`** or **`Up` / `Down`**: Move your left paddle up and down.
  * **`[SPACE]`**: Live-cycle through active GPU shaders (`Grid` ➔ `Rainbow` ➔ `Pulse` ➔ `Vignette` ➔ `None`).
  * **`[R]`**: Reset game scores to `0 - 0` and reset ball position.
  * **`[ESC]` or `[Q]`**: Exit game cleanly.
* **How It Works**:
  1. The compiler detects the `pong` keyword and links `run_pong_game()`.
  2. The game loop polls continuous keyboard input using `SDL_GetKeyboardState`.
  3. The right paddle AI tracks the ball's Y-coordinate at a capped velocity (`pong_ai_speed = 5.2f`).
  4. The physics engine checks **Axis-Aligned Bounding Box (AABB) vs. Circle** collisions:
     * When the ball strikes a paddle, horizontal velocity reverses and accelerates by 5% (`vx = -vx * 1.05f`).
     * Vertical velocity receives an angular deflection proportional to where the ball strikes the paddle relative to its center.
  5. When a goal is scored, scores update and an LED digital scoreboard is drawn at the top using 7-segment bitmask quads with zero external font dependencies.

---

### 5. World3D: 3D First-Person POV Environment ([`world3d.my`](file:///home/eldho/Eldho/SEM 3/uniporjects/shadercompailer/world3d.my))
* **Launch Command**:
  ```bash
  ./mycompiler world3d.my && ./final_game
  ```
* **Controls**:
  * **`W` / `S`**: Walk forward and backward in the direction you are looking.
  * **`A` / `D`**: Strafe left and right.
  * **`Left` / `Right` Arrow Keys** or **`Q` / `E`** or **Mouse**: Turn camera smoothly in 360°.
  * **`[M]`**: Toggle mouse grab on/off.
  * **`[SPACE]`**: Cycle live shaders and ambient room lighting.
  * **`[ESC]`**: Exit back to desktop.
* **How It Works**:
  1. The compiler detects `mode_3d` and calls `run_3d_game()`.
  2. Enables hardware 3D depth-buffering (`glEnable(GL_DEPTH_TEST)`) and perspective projection (`gluPerspective(65.0, aspect, 0.1, 200.0)`).
  3. Sets player camera at an eye-level height of $1.6\text{m}$ (`gluLookAt`).
  4. Renders a 40-unit checkerboard floor grid (`floor 40 #161a2e #0e1224`) and 4 boundary walls (`walls 40 5 #0f3460`).
  5. Renders 3D shaded cubes (`cube`) and monolith pillars (`pillar`) with directional face lighting and crisp wireframe edge outlines.
  6. Renders a continuously rotating gold crystal floating at the center of the room.
  7. **Collision & Sliding**: Bounding-box player cylinder collision prevents the camera from clipping through perimeter walls or 3D cubes, smoothly sliding along surfaces.
  8. Overlays a 2D crosshair (`+`) and status strip on top of the 3D scene.

---

## 📖 Complete DSL Language & Syntax Reference

### 1. Window & Canvas
| Command | Arguments | Description |
|---|---|---|
| `window` | `<w> <h>` | Sets window dimensions in pixels (e.g. `window 900 700`) |
| `title` | `<string>` | Sets window header title (e.g. `title "Cyberpunk World"`) |
| `bg` / `background` | `<color>` | Sets background clear color (`bg #0f172a`, `bg navy`, `bg 20 20 40`) |

### 2. 3D First-Person POV Engine
| Command | Arguments | Description |
|---|---|---|
| `mode_3d` / `3d` | *none* | Activates 3D first-person perspective rendering with depth testing & collision |
| `floor` | `<size> [<color1>] [<color2>]` | Generates a 3D checkerboard floor grid |
| `walls` | `<size> <height> [<color>]` | Builds 4 surrounding enclosure room walls with collision |
| `cube` | `<x> <y> <z> <size> [<color>]` | Places a directional-shaded 3D solid cube with collision |
| `pillar` | `<x> <z> <width> <height> [<color>]` | Places a 3D architectural pillar |

### 3. 2D Interactive Pong Game Engine
| Command | Arguments | Description |
|---|---|---|
| `pong` | *none* | Activates interactive 2D Pong game loop with AI, collision & scoreboard |
| `paddle` | `[<color>]` or `[<x> <y> <w> <h> <color>]` | Customizes player paddle size/color |
| `ai_paddle` | `[<color>]` or `[<x> <y> <w> <h> <color>]` | Customizes AI paddle size/color |
| `ball` | `[<color>]` or `[<x> <y> <radius> <color>]` | Customizes ball size/color |

### 4. GPU Shaders
| Command | Arguments | Description |
|---|---|---|
| `shader` | `grid`, `rainbow`, `pulse`, `vignette`, `none` | Activates hardware GLSL 1.20 fragment shader |
| `custom_shader` | `<filepath>` | Compiles and links an external `.frag` shader file |

### 5. Colors & Styles
* **35+ Built-in Named Colors**:
  `red`, `green`, `blue`, `white`, `black`, `yellow`, `cyan`, `magenta`, `orange`, `purple`, `pink`, `gray`, `darkgray`, `lightgray`, `gold`, `silver`, `brown`, `lime`, `teal`, `navy`, `violet`, `indigo`, `coral`, `crimson`, `skyblue`, `turquoise`, `maroon`, `olive`, `mint`, `peach`, `lavender`, `amber`, `rose`.
* **Hex Color Formats**: `#ff007f`, `#00ffff`, `#1e293b`.
* **RGB / RGBA Numbers**: `color 255 128 0` (RGB) or `color 255 128 0 180` (RGBA with transparency).
* **Alpha Adjustment**: `alpha <0-255>` (e.g. `alpha 200`).
* **Stroke & Point Size**: `line_width <w>` and `point_size <s>`.

### 6. Gradients & Matrix Transformations
* `gradient_rect <x> <y> <w> <h> <top_color> <bottom_color>`: Vertical 2-color gradient.
* `gradient_h_rect <x> <y> <w> <h> <left_color> <right_color>`: Horizontal 2-color gradient.
* `gradient_triangle <x1> <y1> <c1> <x2> <y2> <c2> <x3> <y3> <c3>`: 3-color interpolated triangle.
* `rotate <deg>`: Rotates coordinates around the active origin.
* `translate <dx> <dy>`: Moves coordinate origin in 2D space.
* `scale <sx> <sy>`: Scales subsequent shapes.
* `push` / `pop`: Saves (`glPushMatrix`) and restores (`glPopMatrix`) coordinate matrices.
* `reset_transform`: Resets transform back to identity (`glLoadIdentity`).

### 7. 2D Geometric Primitives
* **Rectangles & Squares**: `rect`, `rect_outline`, `center_rect`, `center_rect_outline`, `square`, `square_outline`, `center_square`, `center_square_outline`, `rounded_rect`, `rounded_rect_outline`.
* **Circles & Ellipses**: `circle`, `fill_circle`, `center_circle`, `fill_center_circle`, `ellipse`, `fill_ellipse`.
* **Triangles, Quads & Polygons**: `triangle`, `fill_triangle`, `quad`, `fill_quad`, `polygon` (regular polygons), `fill_polygon`, `star`, `fill_star`.
* **Lines & Points**: `line`, `point`, `dot`.

---

## 🔬 Technical Deep Dive & Architecture

### Coordinate Conventions
* **2D Mode**: $(0,0)$ is top-left. $X$ increases to the right; $Y$ increases downwards.
* **3D Mode**: Right-handed Cartesian coordinates. $+X$ is East, $+Y$ is Up, $+Z$ is South (toward camera).
* **Angles**: DSL commands accept degrees; the runtime converts to radians internally.
* **Color Channels**: DSL accepts $0\dots255$; internal shaders use normalized $[0.0, 1.0]$.

### Collision Mathematics
* **Pong AABB vs. Circle**:
  $$\text{Collision if: } |x_{\text{ball}} - x_{\text{paddle\_center}}| \le \frac{w}{2} + r \quad \text{and} \quad |y_{\text{ball}} - y_{\text{paddle\_center}}| \le \frac{h}{2} + r$$
* **3D Wall Sliding**:
  Candidate positions $(nx, nz)$ are evaluated independently. If an obstacle boundary is breached along the X-axis, $nx$ is clamped to the surface while $nz$ remains free, creating smooth wall-sliding motion.

---

## 🎓 University Review & Viva Presentation Guide

When presenting this project to your professor, use this 4-step narrative:

1. **The Motivation**:
   > *"Sir/Ma'am, standard graphics in C require managing window handles, pointers, GLSL compilation, and matrix math. We developed a Domain-Specific Language that abstracts these complexities into declarative commands like `mode_3d`, `shader grid`, and `pong`."*
2. **The Transpiler Pipeline**:
   > *"Our C++ compiler parses `.my` source files, generates intermediate C code (`generated_game.c`), and automatically invokes GCC linking our custom OpenGL 2 runtime engine (`graphics_runtime.h`)."*
3. **Milestone Achievements**:
   > *"We evolved our Review 1 prototype from basic 2D shapes into a full graphics suite featuring GPU fragment shaders, 35+ named colors, multi-color gradients, an interactive Pong game with AI and collision, and a navigable 3D first-person environment with depth-buffering."*
4. **Live Demonstrations to Show Faculty**:
   * Show 2D vector art: `./mycompiler showcase.my && ./final_game`
   * Show real-time 2D game: `./mycompiler pong.my && ./final_game` *(Press Spacebar live to switch shaders!)*
   * Show 3D world: `./mycompiler world3d.my && ./final_game` *(Walk around with WASD and look with mouse!)*
