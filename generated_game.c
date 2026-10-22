// --- AUTOMATICALLY GENERATED OPENGL 2 GRAPHICS CODE ---
#include "graphics_runtime.h"

int main() {
    create_window_with_title(800, 600, "Cyberpunk Pong - Player vs AI (OpenGL 2)");
    set_background(6, 6, 20);
    set_shader("grid");
    set_pong_paddle_color(0, 255, 255);
    set_pong_ai_color(255, 0, 255);
    set_pong_ball_color(255, 215, 0);
    run_pong_game();
    return 0;
}
