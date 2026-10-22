#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Function to draw a circle
void drawCircle(SDL_Renderer *renderer, int cx, int cy, int radius)
{
    int x = radius;
    int y = 0;

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);

        y++;

        if (y * y + x * x > radius * radius)
        {
            x--;
        }
    }
}

// Function to draw a triangle
void drawTriangle(SDL_Renderer *renderer,
                  int x1, int y1,
                  int x2, int y2,
                  int x3, int y3)
{
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow(
        "SDL2 Shapes",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL)
    {
        printf("Window Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL)
    {
        printf("Renderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running)
    {
        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // --------------------------------
        // Draw TRIANGLE
        // --------------------------------

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        drawTriangle(
            renderer,
            150, 400,   // Point 1
            300, 150,   // Point 2
            450, 400    // Point 3
        );

        // --------------------------------
        // Draw CIRCLE
        // --------------------------------

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        drawCircle(
            renderer,
            600, 250,   // Center
            100         // Radius
        );

        // --------------------------------
        // Draw LINES
        // --------------------------------

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        SDL_RenderDrawLine(
            renderer,
            100, 500,
            700, 500
        );

        SDL_RenderDrawLine(
            renderer,
            100, 520,
            700, 550
        );

        // Display everything
        SDL_RenderPresent(renderer);

        // Small delay
        SDL_Delay(16);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}