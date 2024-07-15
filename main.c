#include "cpu_core.h"
#include "cpu_init.h"
#include "SDL.h"

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

void final_cleanup(sdl_t sdl)
{
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
    return;
}

bool init_sdl(sdl_t *sdl)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        SDL_Log("SDL Subsystem not initialised! %s\n", SDL_GetError());
        return false;
    }
    sdl->window = SDL_CreateWindow("CHIP8 Emulator",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   100,
                                   100,
                                   0);

    if (!sdl->window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    // RENDERER
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer)
    {
        printf("renderer could not be created! %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void clear_screen(sdl_t sdl)
{
    uint8_t r = 0xFF;
    uint8_t g = 0xFF;
    uint8_t b = 0xFF;
    uint8_t a = 0xFF;

    SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
    SDL_RenderClear(sdl.renderer);
}

void handle_input(CPU_t* cpu)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            cpu->state = QUIT;
            return;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_SPACE:
                if (cpu->state == RUNNING)
                {
                    cpu->state = PAUSED;
                    puts("==========PAUSED============");
                }
                else
                    cpu->state = RUNNING;

                break;
            }
            break;
        }
    }
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    if (argc < 2)
    {
        printf("Usage: ./%s <rom_name>\n", argv[0]);
        return -1;
    }
    CPU_t cpu = {0};
    CONFIG_t config = {0};
    sdl_t sdl = {0};
    reset_cpu(&cpu);
    // read ROM functionality
    init_config(&config, argv[1]);
    init_cpu(&cpu, config);

    init_sdl(&sdl);
    clear_screen(sdl);

    while(cpu.state != QUIT)
    {
        handle_input(&cpu);
        if(cpu.state == PAUSED)
            continue;
    }

    final_cleanup(sdl);
    #ifdef DEBUG
    printf("DEBUG");
    hex_dump(cpu);
    #endif
    return 0;
}