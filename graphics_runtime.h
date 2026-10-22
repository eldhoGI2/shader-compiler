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
static int current_shader_idx = 0;
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
// Pong Game State Variables
// ==========================================
static float pong_paddle_x = 40.0f;
static float pong_paddle_y = 250.0f;
static float pong_paddle_w = 18.0f;
static float pong_paddle_h = 100.0f;
static float pong_paddle_speed = 7.5f;
static uint8_t pong_paddle_r = 0, pong_paddle_g = 255, pong_paddle_b = 255; // Cyan

static float pong_ai_x = 742.0f;
static float pong_ai_y = 250.0f;
static float pong_ai_w = 18.0f;
static float pong_ai_h = 100.0f;
static float pong_ai_speed = 5.2f;
static uint8_t pong_ai_r = 255, pong_ai_g = 0, pong_ai_b = 255; // Magenta

static float pong_ball_x = 400.0f;
static float pong_ball_y = 300.0f;
static float pong_ball_radius = 10.0f;
static float pong_ball_vx = 6.0f;
static float pong_ball_vy = 3.0f;
static uint8_t pong_ball_r = 255, pong_ball_g = 215, pong_ball_b = 0; // Gold

static int pong_score_player = 0;
static int pong_score_ai = 0;

// 7-Segment Display Bitmasks (0-9)
static const uint8_t DIGITS[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

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

static inline void cycle_next_shader() {
    int max_idx = (custom_shader_prog != 0) ? 5 : 4;
    current_shader_idx = (current_shader_idx + 1) % (max_idx + 1);

    switch (current_shader_idx) {
        case 0: set_shader("none"); printf("[SHADER] NONE\n"); break;
        case 1: set_shader("grid"); printf("[SHADER] GRID (Cyberpunk Matrix)\n"); break;
        case 2: set_shader("rainbow"); printf("[SHADER] RAINBOW (Chromatic Wave)\n"); break;
        case 3: set_shader("pulse"); printf("[SHADER] PULSE (Neon Energy Ring)\n"); break;
        case 4: set_shader("vignette"); printf("[SHADER] VIGNETTE (Cinematic Edge)\n"); break;
        case 5: set_shader("custom"); printf("[SHADER] CUSTOM (External GLSL)\n"); break;
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
// Digital 7-Segment Score Display
// ==========================================
static inline void draw_digital_digit(float x, float y, int digit, float size) {
    if (digit < 0 || digit > 9) return;
    uint8_t mask = DIGITS[digit];
    float w = size * 0.55f;
    float h = size;
    float t = size * 0.12f;

    glBegin(GL_QUADS);
    if (mask & 0x01) { // 0: Top
        glVertex2f(x + t, y); glVertex2f(x + w - t, y);
        glVertex2f(x + w - t, y + t); glVertex2f(x + t, y + t);
    }
    if (mask & 0x02) { // 1: Top-Right
        glVertex2f(x + w - t, y + t); glVertex2f(x + w, y + t);
        glVertex2f(x + w, y + h * 0.5f); glVertex2f(x + w - t, y + h * 0.5f);
    }
    if (mask & 0x04) { // 2: Bottom-Right
        glVertex2f(x + w - t, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
        glVertex2f(x + w, y + h - t); glVertex2f(x + w - t, y + h - t);
    }
    if (mask & 0x08) { // 3: Bottom
        glVertex2f(x + t, y + h - t); glVertex2f(x + w - t, y + h - t);
        glVertex2f(x + w - t, y + h); glVertex2f(x + t, y + h);
    }
    if (mask & 0x10) { // 4: Bottom-Left
        glVertex2f(x, y + h * 0.5f); glVertex2f(x + t, y + h * 0.5f);
        glVertex2f(x + t, y + h - t); glVertex2f(x, y + h - t);
    }
    if (mask & 0x20) { // 5: Top-Left
        glVertex2f(x, y + t); glVertex2f(x + t, y + t);
        glVertex2f(x + t, y + h * 0.5f); glVertex2f(x, y + h * 0.5f);
    }
    if (mask & 0x40) { // 6: Middle
        glVertex2f(x + t, y + h * 0.5f - t * 0.5f); glVertex2f(x + w - t, y + h * 0.5f - t * 0.5f);
        glVertex2f(x + w - t, y + h * 0.5f + t * 0.5f); glVertex2f(x + t, y + h * 0.5f + t * 0.5f);
    }
    glEnd();
}

static inline void draw_digital_number(float x, float y, int number, float size) {
    if (number < 0) number = 0;
    if (number >= 10) {
        draw_digital_digit(x - size * 0.65f, y, (number / 10) % 10, size);
    }
    draw_digital_digit(x, y, number % 10, size);
}

// ==========================================
// Pong Configuration Setters (from DSL)
// ==========================================
static inline void set_pong_paddle(int x, int y, int w, int h) {
    pong_paddle_x = (float)x; pong_paddle_y = (float)y;
    pong_paddle_w = (float)w; pong_paddle_h = (float)h;
}

static inline void set_pong_paddle_color(int r, int g, int b) {
    pong_paddle_r = (uint8_t)r; pong_paddle_g = (uint8_t)g; pong_paddle_b = (uint8_t)b;
}

static inline void set_pong_ai(int x, int y, int w, int h) {
    pong_ai_x = (float)x; pong_ai_y = (float)y;
    pong_ai_w = (float)w; pong_ai_h = (float)h;
}

static inline void set_pong_ai_color(int r, int g, int b) {
    pong_ai_r = (uint8_t)r; pong_ai_g = (uint8_t)g; pong_ai_b = (uint8_t)b;
}

static inline void set_pong_ball(int x, int y, int radius) {
    pong_ball_x = (float)x; pong_ball_y = (float)y;
    pong_ball_radius = (float)radius;
}

static inline void set_pong_ball_color(int r, int g, int b) {
    pong_ball_r = (uint8_t)r; pong_ball_g = (uint8_t)g; pong_ball_b = (uint8_t)b;
}

static inline void pong_reset_ball(int direction) {
    pong_ball_x = screen_width * 0.5f;
    pong_ball_y = screen_height * 0.5f;
    pong_ball_vx = direction * 6.5f;
    pong_ball_vy = (((rand() % 100) / 50.0f) - 1.0f) * 4.0f;
    if (fabsf(pong_ball_vy) < 1.5f) {
        pong_ball_vy = (pong_ball_vy < 0 ? -2.5f : 2.5f);
    }
}

// ==========================================
// Interactive Pong Game Loop
// ==========================================
static inline void run_pong_game() {
    glEndList();

    // Auto-adjust positions if screen dimensions were customized
    pong_ai_x = screen_width - pong_paddle_x - pong_ai_w;

    printf("\n=======================================================\n");
    printf("   🏓 PONG GAME RUNNING (OpenGL 2 + DSL)              \n");
    printf("   - Player 1: [W] / [S] or [UP] / [DOWN] to Move     \n");
    printf("   - Press [SPACE] to cycle live OpenGL 2 shaders!    \n");
    printf("   - Press [R] to reset score                         \n");
    printf("   - Press [ESC] to exit                              \n");
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
                } else if (event.key.keysym.sym == SDLK_r) {
                    pong_score_player = 0;
                    pong_score_ai = 0;
                    pong_reset_ball(1);
                    printf("[PONG] Scores reset!\n");
                }
            }
        }

        // 1. Continuous Keyboard Input (Player Movement)
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
            pong_paddle_y -= pong_paddle_speed;
        }
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
            pong_paddle_y += pong_paddle_speed;
        }

        // Clamp Player Paddle inside window
        if (pong_paddle_y < 12.0f) pong_paddle_y = 12.0f;
        if (pong_paddle_y + pong_paddle_h > screen_height - 12.0f) {
            pong_paddle_y = screen_height - 12.0f - pong_paddle_h;
        }

        // 2. Simple Smooth AI Paddle Logic
        float ai_center = pong_ai_y + pong_ai_h * 0.5f;
        if (ai_center < pong_ball_y - 12.0f) {
            pong_ai_y += pong_ai_speed;
        } else if (ai_center > pong_ball_y + 12.0f) {
            pong_ai_y -= pong_ai_speed;
        }
        if (pong_ai_y < 12.0f) pong_ai_y = 12.0f;
        if (pong_ai_y + pong_ai_h > screen_height - 12.0f) {
            pong_ai_y = screen_height - 12.0f - pong_ai_h;
        }

        // 3. Ball Physics & Collision Detection
        pong_ball_x += pong_ball_vx;
        pong_ball_y += pong_ball_vy;

        // Top & Bottom Boundary Collision
        if (pong_ball_y - pong_ball_radius <= 8.0f) {
            pong_ball_y = 8.0f + pong_ball_radius;
            pong_ball_vy = -pong_ball_vy;
        } else if (pong_ball_y + pong_ball_radius >= screen_height - 8.0f) {
            pong_ball_y = screen_height - 8.0f - pong_ball_radius;
            pong_ball_vy = -pong_ball_vy;
        }

        // Left Paddle Collision (Player) - Circle vs AABB Bounding Box
        if (pong_ball_vx < 0.0f &&
            pong_ball_x - pong_ball_radius <= pong_paddle_x + pong_paddle_w &&
            pong_ball_x + pong_ball_radius >= pong_paddle_x &&
            pong_ball_y >= pong_paddle_y - pong_ball_radius &&
            pong_ball_y <= pong_paddle_y + pong_paddle_h + pong_ball_radius) {

            pong_ball_x = pong_paddle_x + pong_paddle_w + pong_ball_radius;
            pong_ball_vx = -pong_ball_vx * 1.05f; // Slight acceleration per hit
            float hit_offset = (pong_ball_y - (pong_paddle_y + pong_paddle_h * 0.5f)) / (pong_paddle_h * 0.5f);
            pong_ball_vy = hit_offset * 6.5f;
        }

        // Right Paddle Collision (AI) - Circle vs AABB Bounding Box
        if (pong_ball_vx > 0.0f &&
            pong_ball_x + pong_ball_radius >= pong_ai_x &&
            pong_ball_x - pong_ball_radius <= pong_ai_x + pong_ai_w &&
            pong_ball_y >= pong_ai_y - pong_ball_radius &&
            pong_ball_y <= pong_ai_y + pong_ai_h + pong_ball_radius) {

            pong_ball_x = pong_ai_x - pong_ball_radius;
            pong_ball_vx = -pong_ball_vx * 1.05f;
            float hit_offset = (pong_ball_y - (pong_ai_y + pong_ai_h * 0.5f)) / (pong_ai_h * 0.5f);
            pong_ball_vy = hit_offset * 6.5f;
        }

        // Scoring & Ball Reset
        if (pong_ball_x < 0.0f) {
            pong_score_ai++;
            printf("[GOAL] Computer scored! (%d - %d)\n", pong_score_player, pong_score_ai);
            pong_reset_ball(1);
        } else if (pong_ball_x > screen_width) {
            pong_score_player++;
            printf("[GOAL] Player scored! (%d - %d)\n", pong_score_player, pong_score_ai);
            pong_reset_ball(-1);
        }

        // 4. Render Stage
        glClearColor(bg_r, bg_g, bg_b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // A. Background Shader (if active)
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

        // B. Replay any custom user-drawn decorations from DSL
        glCallList(scene_display_list);

        // C. Center Court Dotted Net
        glColor4ub(255, 255, 255, 90);
        float net_x = screen_width * 0.5f - 2.0f;
        for (float y = 10.0f; y < screen_height - 10.0f; y += 30.0f) {
            draw_rectangle((int)net_x, (int)y, 4, 16);
        }

        // D. Digital Scoreboards at Top
        glColor4ub(0, 255, 255, 200); // Player score cyan
        draw_digital_number(screen_width * 0.35f, 35.0f, pong_score_player, 40.0f);

        glColor4ub(255, 0, 255, 200); // AI score magenta
        draw_digital_number(screen_width * 0.65f, 35.0f, pong_score_ai, 40.0f);

        // E. Draw Player Paddle (Rounded Rect)
        glColor4ub(pong_paddle_r, pong_paddle_g, pong_paddle_b, 255);
        draw_fill_rounded_rect((int)pong_paddle_x, (int)pong_paddle_y,
                               (int)pong_paddle_w, (int)pong_paddle_h, 6);

        // F. Draw AI Paddle (Rounded Rect)
        glColor4ub(pong_ai_r, pong_ai_g, pong_ai_b, 255);
        draw_fill_rounded_rect((int)pong_ai_x, (int)pong_ai_y,
                               (int)pong_ai_w, (int)pong_ai_h, 6);

        // G. Draw Ball (Smooth Circle)
        glColor4ub(pong_ball_r, pong_ball_g, pong_ball_b, 255);
        draw_fill_circle((int)pong_ball_x, (int)pong_ball_y, (int)pong_ball_radius);

        // H. Top & Bottom Court Border Lines
        glColor4ub(255, 255, 255, 120);
        glLineWidth(3.0f);
        draw_line(0, 4, screen_width, 4);
        draw_line(0, screen_height - 4, screen_width, screen_height - 4);

        SDL_GL_SwapWindow(window);
        SDL_Delay(16); // ~60 FPS
    }

    if (scene_display_list != 0) glDeleteLists(scene_display_list, 1);
    if (active_shader_prog != 0 && active_shader_prog != custom_shader_prog) glDeleteProgram(active_shader_prog);
    if (custom_shader_prog != 0) glDeleteProgram(custom_shader_prog);
    if (gl_context) SDL_GL_DeleteContext(gl_context);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

// ==========================================
// Static Showcase Render & Wait
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

    if (scene_display_list != 0) glDeleteLists(scene_display_list, 1);
    if (active_shader_prog != 0 && active_shader_prog != custom_shader_prog) glDeleteProgram(active_shader_prog);
    if (custom_shader_prog != 0) glDeleteProgram(custom_shader_prog);
    if (gl_context) SDL_GL_DeleteContext(gl_context);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

#endif // GRAPHICS_RUNTIME_H
