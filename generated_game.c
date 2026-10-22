// --- AUTOMATICALLY GENERATED C CODE ---
#include "graphics_runtime.h"

int main() {
    create_window(800, 600);
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
