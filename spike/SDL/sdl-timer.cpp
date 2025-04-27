#include <iostream>

#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_TimerID timerId;

Uint32 callbackFunc(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    // https://wiki.libsdl.org/SDL3/SDL_TimerCallback
    SDL_Log("1s has passed! interval = %u", interval);
    interval += 500U;
    return interval;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_EVENTS))
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    // https://wiki.libsdl.org/SDL3/SDL_AddTimer
    timerId = SDL_AddTimer(1000U, callbackFunc, nullptr);
    if (timerId == 0)
    {
        std::cerr << "SDL_AddTimer error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
    {
        const SDL_KeyboardEvent &eventKey = event->key;
        const SDL_Scancode &scancode = eventKey.scancode;
        SDL_Log("Key pressed: %d", scancode);
        switch (scancode)
        {
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_Q:
            return SDL_APP_SUCCESS;
        }
        break;
    }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (!SDL_RemoveTimer(timerId)) // https://wiki.libsdl.org/SDL3/SDL_RemoveTimer
    {
        std::cerr << "SDL_RemoveTimer error: " << SDL_GetError() << std::endl;
    }
}
