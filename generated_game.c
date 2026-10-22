// --- AUTOMATICALLY GENERATED OPENGL 2 GRAPHICS CODE ---
#include "graphics_runtime.h"

int main() {
    create_window_with_title(900, 700, "Cyberpunk Matrix (OpenGL 2 + Custom Shader)");
    set_background(5, 5, 16);
    load_custom_shader_file("plasma.frag");
    draw_gradient_rect(50, 50, 800, 250, 0, 255, 255, 255, 255, 0, 127, 255);
    set_color_rgba(255, 234, 0, 255);
    draw_fill_circle(450, 300, 90);
    set_color_rgba(255, 0, 127, 255);
    set_alpha(180);
    draw_fill_circle(450, 300, 75);
    set_color_rgba(5, 5, 16, 255);
    set_line_width(4);
    draw_line(360, 270, 540, 270);
    draw_line(370, 290, 530, 290);
    draw_line(390, 310, 510, 310);
    draw_line(410, 330, 490, 330);
    draw_gradient_h_rect(50, 390, 800, 240, 128, 0, 128, 255, 0, 255, 255, 255);
    set_color_rgba(0, 255, 255, 255);
    set_line_width(2);
    draw_line(450, 390, 100, 630);
    draw_line(450, 390, 250, 630);
    draw_line(450, 390, 450, 630);
    draw_line(450, 390, 650, 630);
    draw_line(450, 390, 800, 630);
    draw_line(100, 440, 800, 440);
    draw_line(100, 490, 800, 490);
    draw_line(100, 550, 800, 550);
    draw_line(100, 620, 800, 620);
    draw_gradient_triangle(160, 320, 255, 0, 0, 255, 220, 220, 50, 205, 50, 255, 280, 320, 0, 0, 255, 255);
    push_transform();
    apply_translate(700, 200);
    apply_rotate(45);
    set_color_rgba(0, 255, 255, 255);
    draw_square_outline(-40, -40, 80);
    set_color_rgba(255, 0, 255, 255);
    set_alpha(160);
    pop_transform();
    push_transform();
    apply_translate(700, 200);
    apply_rotate(15);
    set_color_rgba(255, 215, 0, 255);
    set_line_width(2);
    draw_star(0, 0, 50, 20, 6);
    pop_transform();
    set_color_rgba(0, 255, 255, 255);
    set_line_width(3);
    draw_rounded_rect(40, 40, 820, 600, 16);
    render_and_wait();
    return 0;
}
