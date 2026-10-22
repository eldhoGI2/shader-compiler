#ifndef GRAPHICS_RUNTIME_H
#define GRAPHICS_RUNTIME_H

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==========================================
// Global Graphics Variables & OpenGL State
// ==========================================
static SDL_Window* window = NULL;
static SDL_GLContext gl_context = NULL;
static int screen_width = 800;
static int screen_height = 600;
static GLuint scene_display_list = 0;
static GLuint active_shader_prog = 0;
static bool has_shader = false;

// Shader cycling state
static int current_shader_idx = 0; // 0=none, 1=grid, 2=rainbow, 3=pulse, 4=vignette, 5=custom
static GLuint custom_shader_prog = 0;

// Active rendering color (RGBA)
static uint8_t cur_r = 255;
static uint8_t cur_g = 255;
static uint8_t cur_b = 255;
static uint8_t cur_a = 255;

// Background clear color (RGB normalized 0.0 - 1.0)
static float bg_r = 0.0f;
static float bg_g = 0.0f;
static float bg_b = 0.0f;

// ==========================================
// GLSL Shader Helper Functions
// ==========================================
static GLuint compile_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, sizeof(log), NULL, log);
        fprintf(stderr, "[OPENGL SHADER ERROR]: %s\n", log);
    }
    return s;
}

static GLuint create_program_from_sources(const char* vs_code, const char* fs_code) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_code);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_code);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint linked = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), NULL, log);
        fprintf(stderr, "[OPENGL PROGRAM LINK ERROR]: %s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

static const char* default_vs =
    "#version 120\n"
    "void main() {\n"
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n";

static const char* rainbow_fs =
    "#version 120\n"
    "uniform float u_time;\n"
    "uniform vec2 u_resolution;\n"
    "void main() {\n"
    "    vec2 st = gl_FragCoord.xy / u_resolution.xy;\n"
    "    vec3 col = 0.5 + 0.5 * cos(u_time * 1.5 + st.xyx + vec3(0.0, 2.0, 4.0));\n"
    "    gl_FragColor = vec4(col * 0.45, 1.0);\n"
    "}\n";

static const char* grid_fs =
    "#version 120\n"
    "uniform float u_time;\n"
    "uniform vec2 u_resolution;\n"
    "void main() {\n"
    "    vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution) / u_resolution.y;\n"
    "    vec2 g = abs(fract(uv * 12.0 + vec2(0.0, u_time * 0.25)) - 0.5);\n"
    "    float line = smoothstep(0.04, 0.0, min(g.x, g.y));\n"
    "    vec3 bg = vec3(0.04, 0.06, 0.12);\n"
    "    vec3 neon = vec3(0.0, 0.8, 1.0) * line;\n"
    "    gl_FragColor = vec4(bg + neon, 1.0);\n"
    "}\n";

static const char* pulse_fs =
    "#version 120\n"
    "uniform float u_time;\n"
    "uniform vec2 u_resolution;\n"
    "void main() {\n"
    "    vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution) / min(u_resolution.x, u_resolution.y);\n"
    "    float dist = length(uv);\n"
    "    float ring = 0.03 / abs(dist - 0.35 - 0.08 * sin(u_time * 3.0));\n"
    "    vec3 col = vec3(1.0, 0.2, 0.6) * ring;\n"
    "    gl_FragColor = vec4(vec3(0.03, 0.02, 0.06) + col * 0.6, 1.0);\n"
    "}\n";

static const char* vignette_fs =
    "#version 120\n"
    "uniform float u_time;\n"
    "uniform vec2 u_resolution;\n"
    "void main() {\n"
    "    vec2 uv = gl_FragCoord.xy / u_resolution.xy;\n"
    "    float d = distance(uv, vec2(0.5, 0.5));\n"
    "    float vig = smoothstep(0.85, 0.15, d);\n"
    "    gl_FragColor = vec4(vec3(0.08, 0.1, 0.16) * vig, 1.0);\n"
    "}\n";

static inline void set_shader(const char* name) {
    if (active_shader_prog != 0 && active_shader_prog != custom_shader_prog) {
        glDeleteProgram(active_shader_prog);
        active_shader_prog = 0;
    }

    if (strcmp(name, "rainbow") == 0) {
        active_shader_prog = create_program_from_sources(default_vs, rainbow_fs);
        has_shader = true;
        current_shader_idx = 2;
    } else if (strcmp(name, "grid") == 0) {
        active_shader_prog = create_program_from_sources(default_vs, grid_fs);
        has_shader = true;
        current_shader_idx = 1;
    } else if (strcmp(name, "pulse") == 0) {
        active_shader_prog = create_program_from_sources(default_vs, pulse_fs);
        has_shader = true;
        current_shader_idx = 3;
    } else if (strcmp(name, "vignette") == 0) {
        active_shader_prog = create_program_from_sources(default_vs, vignette_fs);
        has_shader = true;
        current_shader_idx = 4;
    } else if (strcmp(name, "custom") == 0 && custom_shader_prog != 0) {
        active_shader_prog = custom_shader_prog;
        has_shader = true;
        current_shader_idx = 5;
    } else {
        has_shader = false;
        current_shader_idx = 0;
    }
}

// Load custom GLSL fragment shader from external file
static inline void load_custom_shader_file(const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (!f) {
        fprintf(stderr, "[ERROR] Could not open shader file: %s\n", filepath);
        return;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = (char*)malloc(sz + 1);
    fread(buffer, 1, sz, f);
    buffer[sz] = '\0';
    fclose(f);

    if (custom_shader_prog != 0) {
        glDeleteProgram(custom_shader_prog);
    }

    custom_shader_prog = create_program_from_sources(default_vs, buffer);
    free(buffer);

    if (custom_shader_prog != 0) {
        active_shader_prog = custom_shader_prog;
        has_shader = true;
        current_shader_idx = 5;
        printf("[OPENGL] Loaded custom fragment shader from: %s\n", filepath);
    }
}

// Cycle shaders at runtime via Spacebar
static inline void cycle_next_shader() {
    int max_idx = (custom_shader_prog != 0) ? 5 : 4;
    current_shader_idx = (current_shader_idx + 1) % (max_idx + 1);

    switch (current_shader_idx) {
        case 0:
            set_shader("none");
            printf("[SHADER CYCLED] Mode: NONE (Clean Background)\n");
            break;
        case 1:
            set_shader("grid");
            printf("[SHADER CYCLED] Mode: GRID (Cyberpunk Matrix)\n");
            break;
        case 2:
            set_shader("rainbow");
            printf("[SHADER CYCLED] Mode: RAINBOW (Flowing Chromatic Wave)\n");
            break;
        case 3:
            set_shader("pulse");
            printf("[SHADER CYCLED] Mode: PULSE (Neon Energy Ring)\n");
            break;
        case 4:
            set_shader("vignette");
            printf("[SHADER CYCLED] Mode: VIGNETTE (Cinematic Dark Edges)\n");
            break;
        case 5:
            set_shader("custom");
            printf("[SHADER CYCLED] Mode: CUSTOM (External User GLSL Shader)\n");
            break;
    }
}

// ==========================================
// Window Creation & Setup (OpenGL 2 Context)
// ==========================================
static inline void create_window_with_title(int width, int height, const char* title) {
    screen_width = width;
    screen_height = height;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[ERROR] SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow(
        title ? title : "Shader Program Compiler (OpenGL 2)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "[ERROR] Window creation failed: %s\n", SDL_GetError());
        return;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        fprintf(stderr, "[ERROR] OpenGL Context creation failed: %s\n", SDL_GetError());
        return;
    }

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glClearColor(bg_r, bg_g, bg_b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    scene_display_list = glGenLists(1);
    glNewList(scene_display_list, GL_COMPILE);
}

static inline void create_window(int width, int height) {
    create_window_with_title(width, height, "Shader Program Compiler (OpenGL 2)");
}

// ==========================================
// Colors & Visual Styles
// ==========================================
static inline void set_color(int r, int g, int b) {
    cur_r = (uint8_t)r; cur_g = (uint8_t)g; cur_b = (uint8_t)b;
    glColor4ub(cur_r, cur_g, cur_b, cur_a);
}

static inline void set_color_rgba(int r, int g, int b, int a) {
    cur_r = (uint8_t)r; cur_g = (uint8_t)g; cur_b = (uint8_t)b; cur_a = (uint8_t)a;
    glColor4ub(cur_r, cur_g, cur_b, cur_a);
}

static inline void set_alpha(int a) {
    cur_a = (uint8_t)a;
    glColor4ub(cur_r, cur_g, cur_b, cur_a);
}

static inline void set_background(int r, int g, int b) {
    bg_r = r / 255.0f;
    bg_g = g / 255.0f;
    bg_b = b / 255.0f;
}

static inline void set_line_width(float w) {
    glLineWidth(w);
}

static inline void set_point_size(float s) {
    glPointSize(s);
}

// ==========================================
// Transformations (Matrix Operations)
// ==========================================
static inline void apply_rotate(float angle_degrees) {
    glRotatef(angle_degrees, 0.0f, 0.0f, 1.0f);
}

static inline void apply_translate(float dx, float dy) {
    glTranslatef(dx, dy, 0.0f);
}

static inline void apply_scale(float sx, float sy) {
    glScalef(sx, sy, 1.0f);
}

static inline void push_transform() {
    glPushMatrix();
}

static inline void pop_transform() {
    glPopMatrix();
}

static inline void reset_transform() {
    glLoadIdentity();
}

// ==========================================
// Color Gradients
// ==========================================
// Vertical Gradient Rectangle (top to bottom)
static inline void draw_gradient_rect(int x, int y, int w, int h,
                                      int r1, int g1, int b1, int a1,
                                      int r2, int g2, int b2, int a2) {
    glBegin(GL_QUADS);
    glColor4ub(r1, g1, b1, a1);
    glVertex2f((float)x, (float)y);
    glVertex2f((float)(x + w), (float)y);

    glColor4ub(r2, g2, b2, a2);
    glVertex2f((float)(x + w), (float)(y + h));
    glVertex2f((float)x, (float)(y + h));
    glEnd();
    glColor4ub(cur_r, cur_g, cur_b, cur_a);
}

// Horizontal Gradient Rectangle (left to right)
static inline void draw_gradient_h_rect(int x, int y, int w, int h,
                                        int r1, int g1, int b1, int a1,
                                        int r2, int g2, int b2, int a2) {
    glBegin(GL_QUADS);
    glColor4ub(r1, g1, b1, a1);
    glVertex2f((float)x, (float)y);

    glColor4ub(r2, g2, b2, a2);
    glVertex2f((float)(x + w), (float)y);
    glVertex2f((float)(x + w), (float)(y + h));

    glColor4ub(r1, g1, b1, a1);
    glVertex2f((float)x, (float)(y + h));
    glEnd();
    glColor4ub(cur_r, cur_g, cur_b, cur_a);
}

// Multi-color Gradient Triangle
static inline void draw_gradient_triangle(int x1, int y1, int r1, int g1, int b1, int a1,
                                          int x2, int y2, int r2, int g2, int b2, int a2,
                                          int x3, int y3, int r3, int g3, int b3, int a3) {
    glBegin(GL_TRIANGLES);
    glColor4ub(r1, g1, b1, a1);
    glVertex2f((float)x1, (float)y1);

    glColor4ub(r2, g2, b2, a2);
    glVertex2f((float)x2, (float)y2);

    glColor4ub(r3, g3, b3, a3);
    glVertex2f((float)x3, (float)y3);
    glEnd();
    glColor4ub(cur_r, cur_g, cur_b, cur_a);
}

// ==========================================
// Shapes - Rectangles & Squares
// ==========================================
static inline void draw_rectangle(int x, int y, int w, int h) {
    glBegin(GL_QUADS);
    glVertex2f((float)x, (float)y);
    glVertex2f((float)(x + w), (float)y);
    glVertex2f((float)(x + w), (float)(y + h));
    glVertex2f((float)x, (float)(y + h));
    glEnd();
}

static inline void draw_rect_outline(int x, int y, int w, int h) {
    glBegin(GL_LINE_LOOP);
    glVertex2f((float)x, (float)y);
    glVertex2f((float)(x + w), (float)y);
    glVertex2f((float)(x + w), (float)(y + h));
    glVertex2f((float)x, (float)(y + h));
    glEnd();
}

static inline void draw_center_rectangle(int w, int h) {
    int x = (screen_width - w) / 2;
    int y = (screen_height - h) / 2;
    draw_rectangle(x, y, w, h);
}

static inline void draw_center_rect_outline(int w, int h) {
    int x = (screen_width - w) / 2;
    int y = (screen_height - h) / 2;
    draw_rect_outline(x, y, w, h);
}

static inline void draw_square(int x, int y, int size) {
    draw_rectangle(x, y, size, size);
}

static inline void draw_square_outline(int x, int y, int size) {
    draw_rect_outline(x, y, size, size);
}

static inline void draw_center_square(int size) {
    draw_center_rectangle(size, size);
}

static inline void draw_center_square_outline(int size) {
    draw_center_rect_outline(size, size);
}

// Rounded Rectangle
static inline void draw_fill_rounded_rect(int x, int y, int w, int h, int radius) {
    if (radius <= 0) { draw_rectangle(x, y, w, h); return; }
    int max_r = (w < h ? w : h) / 2;
    if (radius > max_r) radius = max_r;

    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);

    float corners[4][2] = {
        {(float)(x + w - radius), (float)(y + radius)},
        {(float)(x + w - radius), (float)(y + h - radius)},
        {(float)(x + radius), (float)(y + h - radius)},
        {(float)(x + radius), (float)(y + radius)}
    };
    float start_angles[4] = {- (float)M_PI * 0.5f, 0.0f, (float)M_PI * 0.5f, (float)M_PI};

    for (int c = 0; c < 4; c++) {
        for (int i = 0; i <= 10; i++) {
            float th = start_angles[c] + ((float)M_PI * 0.5f) * ((float)i / 10.0f);
            glVertex2f(corners[c][0] + radius * cosf(th), corners[c][1] + radius * sinf(th));
        }
    }
    glVertex2f(corners[0][0] + radius * cosf(start_angles[0]), corners[0][1] + radius * sinf(start_angles[0]));
    glEnd();
}

static inline void draw_rounded_rect(int x, int y, int w, int h, int radius) {
    if (radius <= 0) { draw_rect_outline(x, y, w, h); return; }
    int max_r = (w < h ? w : h) / 2;
    if (radius > max_r) radius = max_r;

    float corners[4][2] = {
        {(float)(x + w - radius), (float)(y + radius)},
        {(float)(x + w - radius), (float)(y + h - radius)},
        {(float)(x + radius), (float)(y + h - radius)},
        {(float)(x + radius), (float)(y + radius)}
    };
    float start_angles[4] = {- (float)M_PI * 0.5f, 0.0f, (float)M_PI * 0.5f, (float)M_PI};

    glBegin(GL_LINE_LOOP);
    for (int c = 0; c < 4; c++) {
        for (int i = 0; i <= 10; i++) {
            float th = start_angles[c] + ((float)M_PI * 0.5f) * ((float)i / 10.0f);
            glVertex2f(corners[c][0] + radius * cosf(th), corners[c][1] + radius * sinf(th));
        }
    }
    glEnd();
}

// ==========================================
// Shapes - Circles & Ellipses
// ==========================================
static inline void draw_circle(int cx, int cy, int radius) {
    int segments = 64;
    if (segments < radius) segments = radius;
    if (segments > 120) segments = 120;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex2f((float)cx + (float)radius * cosf(theta), (float)cy + (float)radius * sinf(theta));
    }
    glEnd();
}

static inline void draw_fill_circle(int cx, int cy, int radius) {
    int segments = 64;
    if (segments < radius) segments = radius;
    if (segments > 120) segments = 120;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)cx, (float)cy);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex2f((float)cx + (float)radius * cosf(theta), (float)cy + (float)radius * sinf(theta));
    }
    glEnd();
}

static inline void draw_center_circle(int radius) {
    draw_circle(screen_width / 2, screen_height / 2, radius);
}

static inline void draw_fill_center_circle(int radius) {
    draw_fill_circle(screen_width / 2, screen_height / 2, radius);
}

static inline void draw_ellipse(int cx, int cy, int rx, int ry) {
    int segments = 64;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex2f((float)cx + (float)rx * cosf(theta), (float)cy + (float)ry * sinf(theta));
    }
    glEnd();
}

static inline void draw_fill_ellipse(int cx, int cy, int rx, int ry) {
    int segments = 64;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)cx, (float)cy);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex2f((float)cx + (float)rx * cosf(theta), (float)cy + (float)ry * sinf(theta));
    }
    glEnd();
}

// ==========================================
// Shapes - Triangles & Polygons
// ==========================================
static inline void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    glBegin(GL_LINE_LOOP);
    glVertex2f((float)x1, (float)y1);
    glVertex2f((float)x2, (float)y2);
    glVertex2f((float)x3, (float)y3);
    glEnd();
}

static inline void draw_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f((float)x1, (float)y1);
    glVertex2f((float)x2, (float)y2);
    glVertex2f((float)x3, (float)y3);
    glEnd();
}

static inline void draw_quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    glBegin(GL_LINE_LOOP);
    glVertex2f((float)x1, (float)y1);
    glVertex2f((float)x2, (float)y2);
    glVertex2f((float)x3, (float)y3);
    glVertex2f((float)x4, (float)y4);
    glEnd();
}

static inline void draw_fill_quad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    glBegin(GL_QUADS);
    glVertex2f((float)x1, (float)y1);
    glVertex2f((float)x2, (float)y2);
    glVertex2f((float)x3, (float)y3);
    glVertex2f((float)x4, (float)y4);
    glEnd();
}

static inline void draw_polygon(int cx, int cy, int radius, int sides) {
    if (sides < 3) sides = 3;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < sides; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)sides - ((float)M_PI * 0.5f);
        glVertex2f((float)cx + (float)radius * cosf(theta), (float)cy + (float)radius * sinf(theta));
    }
    glEnd();
}

static inline void draw_fill_polygon(int cx, int cy, int radius, int sides) {
    if (sides < 3) sides = 3;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)cx, (float)cy);
    for (int i = 0; i <= sides; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)sides - ((float)M_PI * 0.5f);
        glVertex2f((float)cx + (float)radius * cosf(theta), (float)cy + (float)radius * sinf(theta));
    }
    glEnd();
}

static inline void draw_star(int cx, int cy, int outer_r, int inner_r, int points) {
    if (points < 3) points = 3;
    int total_vertices = points * 2;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < total_vertices; i++) {
        float r = (i % 2 == 0) ? (float)outer_r : (float)inner_r;
        float theta = (float)i * (float)M_PI / (float)points - ((float)M_PI * 0.5f);
        glVertex2f((float)cx + r * cosf(theta), (float)cy + r * sinf(theta));
    }
    glEnd();
}

static inline void draw_fill_star(int cx, int cy, int outer_r, int inner_r, int points) {
    if (points < 3) points = 3;
    int total_vertices = points * 2;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f((float)cx, (float)cy);
    for (int i = 0; i <= total_vertices; i++) {
        int idx = i % total_vertices;
        float r = (idx % 2 == 0) ? (float)outer_r : (float)inner_r;
        float theta = (float)idx * (float)M_PI / (float)points - ((float)M_PI * 0.5f);
        glVertex2f((float)cx + r * cosf(theta), (float)cy + r * sinf(theta));
    }
    glEnd();
}

// ==========================================
// Shapes - Lines & Points
// ==========================================
static inline void draw_line(int x1, int y1, int x2, int y2) {
    glBegin(GL_LINES);
    glVertex2f((float)x1, (float)y1);
    glVertex2f((float)x2, (float)y2);
    glEnd();
}

static inline void draw_point(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2f((float)x, (float)y);
    glEnd();
}

// ==========================================
// Main Presentation & Interactive Event Loop
// ==========================================
static inline void render_and_wait() {
    glEndList();

    printf("\n=======================================================\n");
    printf("   OpenGL 2 Shader Application Running!               \n");
    printf("   - Press [SPACE] to cycle live OpenGL 2 shaders!    \n");
    printf("   - Press [ESC] or close window to exit.             \n");
    printf("=======================================================\n\n");

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_SPACE) {
                    cycle_next_shader();
                }
            }
        }

        glClearColor(bg_r, bg_g, bg_b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (has_shader && active_shader_prog != 0) {
            glUseProgram(active_shader_prog);

            GLint loc_time = glGetUniformLocation(active_shader_prog, "u_time");
            GLint loc_res = glGetUniformLocation(active_shader_prog, "u_resolution");

            if (loc_time != -1) {
                float current_time = (float)SDL_GetTicks() / 1000.0f;
                glUniform1f(loc_time, current_time);
            }
            if (loc_res != -1) {
                glUniform2f(loc_res, (float)screen_width, (float)screen_height);
            }

            glBegin(GL_QUADS);
            glVertex2f(0.0f, 0.0f);
            glVertex2f((float)screen_width, 0.0f);
            glVertex2f((float)screen_width, (float)screen_height);
            glVertex2f(0.0f, (float)screen_height);
            glEnd();

            glUseProgram(0);
        }

        glCallList(scene_display_list);

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    if (scene_display_list != 0) {
        glDeleteLists(scene_display_list, 1);
    }
    if (active_shader_prog != 0 && active_shader_prog != custom_shader_prog) {
        glDeleteProgram(active_shader_prog);
    }
    if (custom_shader_prog != 0) {
        glDeleteProgram(custom_shader_prog);
    }
    if (gl_context) {
        SDL_GL_DeleteContext(gl_context);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

#endif // GRAPHICS_RUNTIME_H
