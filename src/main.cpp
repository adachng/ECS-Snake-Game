#include <iostream>

#include <entt/entt.hpp>
#include <sigslot/signal.hpp>
#include <SDL3/SDL.h>

#include <component/delta_time.hpp>
#include <component/key_control.hpp>
#include <component/position.hpp>
#include <component/snake_apple.hpp>
#include <component/snake_boundary_2d.hpp>
#include <component/snake_part_head.hpp>
#include <component/snake_part.hpp>
#include <component/velocity.hpp>

#include <system/translate_2d.hpp>
#include <system/snake_gameplay_system.hpp>

#include "component/delta_time.hpp"
#include "component/key_control.hpp"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

struct AppState
{
    Uint64 previousTick; // for FixedUpdate() equivalent
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
};

namespace Global
{
    static constexpr float SPEED = 1.0f;
    static constexpr float SPEED_UP_FACTOR = 3.0f;
    static constexpr float TICK_UNIT_TRAVELLED = 0.25f; // MUST BE <= 0.5f
    static constexpr int MAP_WIDTH = 20;                // MUST BE >= 1
    static constexpr int MAP_HEIGHT = 20;               // MUST BE >= 1

    static constexpr float MAX_POSSIBLE_SPEED = SPEED * SPEED_UP_FACTOR;
    static constexpr float MAXIMUM_TICK_PERIOD_MS_FLOAT = TICK_UNIT_TRAVELLED * 1000.0f / MAX_POSSIBLE_SPEED;

    static constexpr Uint64 DESIRED_TICK_PERIOD_MS = static_cast<Uint64>(MAXIMUM_TICK_PERIOD_MS_FLOAT - 1.0f);

    entt::registry reg;
    sigslot::signal<entt::registry &> gameplayUpdateSig;
} // namespace Global

void init_gameplay_scene(entt::registry &reg)
{
    auto gameStateEntity = reg.create();
    reg.emplace<DeltaTime>(gameStateEntity, Global::DESIRED_TICK_PERIOD_MS);
    reg.emplace<KeyControl>(gameStateEntity, 'w', false);
    reg.emplace<SnakeBoundary2D>(gameStateEntity, Global::MAP_WIDTH, Global::MAP_HEIGHT);

    auto appleEntity = reg.create();
    const float centerX = static_cast<float>(Global::MAP_WIDTH) / 2.0f;
    const float centerY = static_cast<float>(Global::MAP_HEIGHT) / 2.0f;
    reg.emplace<Position>(appleEntity, centerX, centerY);
    reg.emplace<SnakeApple>(appleEntity);

    auto snakeHeadEntity = reg.create();
    reg.emplace<Position>(snakeHeadEntity, 0.5f, 0.5f);
    reg.emplace<Velocity>(snakeHeadEntity, 0.0f, 0.0f);
    reg.emplace<SnakePartHead>(snakeHeadEntity, 1.0f, 1.5f);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
        return SDL_APP_FAILURE;
    }

    if (!SDL_SetAppMetadata("Snake Game CPP", "1.0", "com.github.adachng.SnakeGameCPP"))
    {
        return SDL_APP_FAILURE;
    }

    *appstate = new AppState();
    AppState *appstateCasted = static_cast<AppState *>(*appstate);

    appstateCasted->window = SDL_CreateWindow("Snake Game CPP", 640, 480, 0);
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

    init_gameplay_scene(Global::reg);
    Global::gameplayUpdateSig.connect(SystemTranslate2D::update);
    Global::gameplayUpdateSig.connect(SnakeGameplaySystem::update);

    appstateCasted->previousTick = SDL_GetTicks(); // for FixedUpdate() equivalent
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *appstateCasted = static_cast<AppState *>(appstate);

    const Uint64 now = SDL_GetTicks();
    while (now - appstateCasted->previousTick >= Global::DESIRED_TICK_PERIOD_MS) // for FixedUpdate() equivalent
    {
        // The reason why is because of how the body follows the head.
        // It is dependent on body entites 2 blocks away in 4 directions from head.
        // If system lags, the head may get detached if deltaTime is not fixed.
        Global::gameplayUpdateSig(Global::reg);
        appstateCasted->previousTick += Global::DESIRED_TICK_PERIOD_MS;

        if (SnakeGameplaySystem::is_game_success(Global::reg))
            SDL_assert(true && false);
        else if (SnakeGameplaySystem::is_game_failure(Global::reg))
            SDL_assert(false && false);

        SDL_Log("\033[2J\033[H");
        SDL_Log("current map at t = %lums", now);
        SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(Global::reg));
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
        switch (scancode)
        {
        case SDL_SCANCODE_ESCAPE:
            return SDL_APP_SUCCESS;
        case SDL_SCANCODE_W:
            SnakeGameplaySystem::Control::up_key_down(Global::reg);
            break;
        case SDL_SCANCODE_A:
            SnakeGameplaySystem::Control::left_key_down(Global::reg);
            break;
        case SDL_SCANCODE_S:
            SnakeGameplaySystem::Control::down_key_down(Global::reg);
            break;
        case SDL_SCANCODE_D:
            SnakeGameplaySystem::Control::right_key_down(Global::reg);
            break;
        case SDL_SCANCODE_SPACE:
            SnakeGameplaySystem::Control::shift_key_down(Global::reg);
            break;
        default:
            break;
        }
        break;
    }
    case SDL_EVENT_KEY_UP:
    {
        const SDL_KeyboardEvent &eventKey = event->key;
        const SDL_Scancode &scancode = eventKey.scancode;
        if (scancode == SDL_SCANCODE_SPACE)
            SnakeGameplaySystem::Control::shift_key_up(Global::reg);
    }
    default:
        break;
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
