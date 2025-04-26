#include <iostream>

#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

struct AppState
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    if (!SDL_SetAppMetadata("SDL Keyboard Test", "1.0", "com.github.adachng.SDLKeyboardTest"))
    {
        return SDL_APP_FAILURE;
    }

    *appstate = new AppState();
    AppState *appstateCasted = static_cast<AppState *>(*appstate);

    appstateCasted->window = SDL_CreateWindow("SDL Keyboard Test", 640, 480, 0);
    if (appstateCasted->window == nullptr)
    {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    appstateCasted->renderer = SDL_CreateRenderer(appstateCasted->window, NULL);
    if (appstateCasted->renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(appstateCasted->window);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *appstateCasted = static_cast<AppState *>(appstate);

    if (appstateCasted->window == nullptr)
    {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    if (appstateCasted->renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(appstateCasted->window);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

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
            return SDL_APP_SUCCESS;
        }
    }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate != NULL)
    {
        AppState *as = static_cast<AppState *>(appstate);
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        delete as;
    }
}
