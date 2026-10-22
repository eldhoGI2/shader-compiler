#ifndef GRAPHICS_RUNTIME_H
#define GRAPHICS_RUNTIME_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Global graphics variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// NEW: We save the screen size so we can calculate the center later!
int screen_width = 0;
int screen_height = 0;

static inline void create_window(int width, int height) {
    screen_width = width;
    screen_height = height;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Review 2 Graphics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Fill background with black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

static inline void set_color(int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

static inline void draw_rectangle(int x, int y, int w, int h) {
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

// NEW: Automatically centers the rectangle on the screen
static inline void draw_center_rectangle(int w, int h) {
    int x = (screen_width - w) / 2;
    int y = (screen_height - h) / 2;
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

// NEW: Draw a line
static inline void draw_line(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

// NEW: Draw a triangle (which is just 3 lines connected!)
static inline void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

static inline void draw_circle(int centreX, int centreY, int radius) {
    const int32_t diameter = (radius * 2);
    int32_t x = (radius - 1), y = 0, tx = 1, ty = 1, error = (tx - diameter);
    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
        if (error <= 0) { ++y; error += ty; ty += 2; }
        if (error > 0) { --x; tx += 2; error += (tx - diameter); }
    }
}

static inline void render_and_wait() {
    SDL_RenderPresent(renderer);
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        SDL_Delay(16); 
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
#endif