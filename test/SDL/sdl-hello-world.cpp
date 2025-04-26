#include <iostream>

#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS // https://wiki.libsdl.org/SDL3/README/main-functions
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    // https://wiki.libsdl.org/SDL3/SDL_AppInit

    if (!SDL_Init(SDL_INIT_EVENTS))
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    std::cout << "Hello";

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // https://wiki.libsdl.org/SDL3/SDL_AppIterate

    std::cout << ", World";

    return SDL_APP_SUCCESS; // SDL_APP_CONTINUE would make this keep iterating
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // https://wiki.libsdl.org/SDL3/SDL_AppEvent
    std::cout << "SDL_AppEvent() ran" << std::endl;
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // https://wiki.libsdl.org/SDL3/SDL_AppQuit
    std::cout << "!" << std::endl;
}
