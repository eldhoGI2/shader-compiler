// --- AUTOMATICALLY GENERATED OPENGL 2 GRAPHICS CODE ---
#include "graphics_runtime.h"

int main() {
    create_window_with_title(900, 700, "3D First-Person POV World (OpenGL 2 Engine)");
    set_background(6, 6, 20);
    set_3d_floor(40, 22, 26, 46, 14, 18, 36);
    set_3d_walls(40, 5, 15, 52, 96);
    add_3d_cube(0, 1, -10, 2, 255, 215, 0);
    add_3d_cube(-8, 1, -6, 2, 0, 255, 255);
    add_3d_cube(8, 1, -6, 2, 255, 0, 255);
    add_3d_cube(-6, 1, 6, 2, 50, 205, 50);
    add_3d_cube(6, 1, 6, 2, 220, 20, 60);
    add_3d_pillar(-12, 0, 2, 5, 233, 69, 96);
    add_3d_pillar(12, 0, 2, 5, 0, 173, 181);
    add_3d_pillar(0, 12, 2, 5, 243, 156, 18);
    run_3d_game();
    return 0;
}
