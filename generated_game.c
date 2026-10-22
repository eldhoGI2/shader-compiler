// --- AUTOMATICALLY GENERATED OPENGL 2 GRAPHICS CODE ---
#include "graphics_runtime.h"

int main() {
    create_window_with_title(800, 600, "Shader Program Compiler (OpenGL 2)");
    set_color(0, 0, 255);
    draw_center_rectangle(600, 400);
    set_color(255, 0, 0);
    draw_circle(400, 300, 100);
    set_color(0, 255, 0);
    draw_triangle(100, 500, 400, 100, 700, 500);
    set_color(255, 255, 255);
    draw_line(0, 0, 800, 600);
    draw_line(0, 600, 800, 0);
    render_and_wait();
    return 0;
}
