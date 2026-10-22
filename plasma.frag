// ==========================================
// Plasma Fractal GLSL 1.20 Fragment Shader
// Compatible with OpenGL 2 Pipeline
// ==========================================
#version 120

uniform float u_time;
uniform vec2 u_resolution;

void main() {
    vec2 p = (gl_FragCoord.xy - 0.5 * u_resolution) / min(u_resolution.x, u_resolution.y);
    float t = u_time * 0.8;

    float v1 = sin(p.x * 10.0 + t);
    float v2 = sin(p.y * 10.0 + t);
    float v3 = sin(p.x * 10.0 + p.y * 10.0 + t);
    float cx = p.x + 0.5 * sin(t / 2.0);
    float cy = p.y + 0.5 * cos(t / 3.0);
    float v4 = sin(sqrt(100.0 * (cx * cx + cy * cy) + 1.0) + t);

    float v = (v1 + v2 + v3 + v4) * 0.25;

    vec3 col = vec3(
        sin(v * 3.14159),
        cos(v * 3.14159),
        sin(v * 3.14159 + 2.0)
    ) * 0.5 + 0.5;

    gl_FragColor = vec4(col * 0.5, 1.0);
}
