#include <iostream>
#include <string>

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
    static constexpr int WINDOW_WIDTH = 640;  // MUST BE > 0
    static constexpr int WINDOW_HEIGHT = 480; // MUST BE > 0
    static constexpr int MAP_MARGIN_PX = 30;  // MUST BE >= 0

    static constexpr float SPEED = 2.0f; // MUST BE >= 0.0f
    static constexpr float SPEED_UP_FACTOR = 5.0f;
    static constexpr float TICK_UNIT_TRAVELLED = 0.25f; // MUST BE <= 0.5f, see Nyquist-Shannon sampling theorem
    static constexpr int MAP_WIDTH = 20;                // MUST BE >= 1
    static constexpr int MAP_HEIGHT = 20;               // MUST BE >= 1

    static constexpr float MAX_POSSIBLE_SPEED = SPEED * SPEED_UP_FACTOR;
    static constexpr float MAXIMUM_TICK_PERIOD_MS_FLOAT = TICK_UNIT_TRAVELLED * 1000.0f / MAX_POSSIBLE_SPEED;

    static constexpr Uint64 DESIRED_TICK_PERIOD_MS = static_cast<Uint64>(MAXIMUM_TICK_PERIOD_MS_FLOAT - 1.0f);

    entt::registry reg;
    sigslot::signal<entt::registry &> gameplayUpdateSig;
} // namespace Global

static SDL_FRect get_centered_boundary(SDL_Window *window, const int &hMargin, const int &vMargin)
{
    SDL_assert(window != nullptr);
    SDL_FRect ret;

    int windowWidth, windowHeight;
    if (!SDL_GetWindowSize(window, &windowWidth, &windowHeight))
    {
        std::cerr << "SDL_GetWindowSize error: " << SDL_GetError() << std::endl;
        return ret;
    }

    if (windowWidth < windowHeight)
    {
        ret.w = static_cast<float>(windowWidth - 2 * hMargin);
    }
    else
    {
        ret.w = static_cast<float>(windowHeight - 2 * vMargin);
    }
    ret.h = ret.w;
    ret.x = static_cast<float>(windowWidth) / 2.0f - ret.w / 2.0f;
    ret.y = static_cast<float>(windowHeight - vMargin) - ret.h;
    return ret;
}

static bool render_map_border(SDL_Window *window, SDL_Renderer *renderer, const int &hMargin, const int &vMargin)
{
    SDL_assert(window != nullptr);
    SDL_assert(renderer != nullptr);
    if (!SDL_SetRenderDrawColor(renderer, 0U, 0U, 0U, SDL_ALPHA_OPAQUE))
    {
        std::cerr << "SDL_SetRenderDrawColor error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!SDL_RenderClear(renderer))
    {
        std::cerr << "SDL_RenderClear error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!SDL_SetRenderDrawColor(renderer, 255U, 255U, 255U, SDL_ALPHA_OPAQUE))
    {
        std::cerr << "SDL_SetRenderDrawColor error: " << SDL_GetError() << std::endl;
        return false;
    }

    int windowWidth, windowHeight;
    if (!SDL_GetWindowSize(window, &windowWidth, &windowHeight))
    {
        std::cerr << "SDL_GetWindowSize error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_FRect mapBoundaryBox = get_centered_boundary(window, hMargin, vMargin);
    if (mapBoundaryBox.w <= 0.0f || mapBoundaryBox.h <= 0.0f)
        return false;

    if (!SDL_RenderRect(renderer, &mapBoundaryBox))
    {
        std::cerr << "SDL_RenderRect error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

static bool render_gameplay_visuals(entt::registry &reg, SDL_Window *window, SDL_Renderer *renderer, const int &hMargin, const int &vMargin)
{
    SDL_assert(window != nullptr);
    SDL_assert(renderer != nullptr);
    if (!render_map_border(window, renderer, hMargin, vMargin))
    {
        return false;
    }

    auto gameplayVecVec = SnakeGameplaySystem::get_map(reg);
    SDL_FRect mapBoundaryBox = get_centered_boundary(window, hMargin, vMargin);
    const float gridHeight = static_cast<float>(mapBoundaryBox.h) / static_cast<float>(gameplayVecVec.size());
    for (int i = 0; i < gameplayVecVec.size(); i++)
    {
        const float gridWidth = static_cast<float>(mapBoundaryBox.w) / static_cast<float>(gameplayVecVec[i].size());
        for (int j = 0; j < gameplayVecVec[i].size(); j++)
        {
            const Uint8 r = (gameplayVecVec[i][j] & SnakeGameplaySystem::MapSlotState::APPLE) ? 255U : 0U;
            const Uint8 g = (gameplayVecVec[i][j] & SnakeGameplaySystem::MapSlotState::SNAKE_BODY) ? 255U : 0U;
            const Uint8 b = (gameplayVecVec[i][j] & SnakeGameplaySystem::MapSlotState::SNAKE_HEAD) ? 255U : 0U;
            const float xCoord = static_cast<float>(j) * gridWidth + mapBoundaryBox.x;
            const float yCoord = static_cast<float>(i) * gridHeight + mapBoundaryBox.y;
            SDL_FRect grid = {xCoord, yCoord, gridWidth, gridHeight};
            if (r > 0 || g > 0 || b > 0)
            {
                if (!SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE))
                {
                    std::cerr << "SDL_SetRenderDrawColor error: " << SDL_GetError() << std::endl;
                    return false;
                }
                if (!SDL_RenderFillRect(renderer, &grid))
                {
                    std::cerr << "SDL_RenderRect error: " << SDL_GetError() << std::endl;
                    return false;
                }
            }
        }
    }

    if (!SDL_SetRenderDrawColor(renderer, 255U, 255U, 255U, SDL_ALPHA_OPAQUE))
    {
        std::cerr << "SDL_SetRenderDrawColor error: " << SDL_GetError() << std::endl;
        return false;
    }

    const float textY = mapBoundaryBox.y + mapBoundaryBox.h + 10.0f;
    std::string textContent;
    if (SnakeGameplaySystem::is_game_success(reg))
    {
        if (!SDL_SetRenderDrawColor(renderer, 0U, 255U, 0U, SDL_ALPHA_OPAQUE))
        {
            std::cerr << "SDL_SetRenderDrawColor error: " << SDL_GetError() << std::endl;
            return false;
        }
        textContent = "Congratulations! You won! Press R to restart. Score: %lu";
    }
    else if (SnakeGameplaySystem::is_game_failure(reg))
    {
        if (!SDL_SetRenderDrawColor(renderer, 255U, 0U, 0U, SDL_ALPHA_OPAQUE))
        {
            std::cerr << "SDL_SetRenderDrawColor error: " << SDL_GetError() << std::endl;
            return false;
        }
        textContent = "Game over! Press R to restart. Score: %lu";
    }
    else
        textContent = "Score: %lu";

    if (!SDL_RenderDebugTextFormat(renderer, mapBoundaryBox.x, textY, textContent.c_str(), SnakeGameplaySystem::get_score(reg)))
    {
        std::cerr << "SDL_RenderDebugTextFormat error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!SDL_RenderPresent(renderer))
    {
        std::cerr << "SDL_RenderPresent error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

static void init_gameplay_scene(entt::registry &reg)
{
    reg.clear();
    auto gameStateEntity = reg.create();
    reg.emplace<DeltaTime>(gameStateEntity, Global::DESIRED_TICK_PERIOD_MS);
    reg.emplace<KeyControl>(gameStateEntity, 'd', false);
    reg.emplace<SnakeBoundary2D>(gameStateEntity, Global::MAP_WIDTH, Global::MAP_HEIGHT);

    auto appleEntity = reg.create();
    const float centerX = static_cast<float>(Global::MAP_WIDTH) / 2.0f;
    const float centerY = static_cast<float>(Global::MAP_HEIGHT) / 2.0f;
    reg.emplace<Position>(appleEntity, centerX, centerY);
    reg.emplace<SnakeApple>(appleEntity);

    auto snakeHeadEntity = reg.create();
    if (centerY >= 1.5f)
        reg.emplace<Position>(snakeHeadEntity, 2.5f, centerY - 1.0f);
    else
        reg.emplace<Position>(snakeHeadEntity, 2.5f, 0.5f);
    reg.emplace<Velocity>(snakeHeadEntity, 0.0f, 0.0f);
    reg.emplace<SnakePartHead>(snakeHeadEntity, Global::SPEED, Global::SPEED_UP_FACTOR);
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

    appstateCasted->window = SDL_CreateWindow("Snake Game CPP", Global::WINDOW_WIDTH, Global::WINDOW_HEIGHT, SDL_WINDOW_INPUT_FOCUS);
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

    render_gameplay_visuals(Global::reg, appstateCasted->window, appstateCasted->renderer, Global::MAP_MARGIN_PX, Global::MAP_MARGIN_PX);

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
        if (!SnakeGameplaySystem::is_game_success(Global::reg) && !SnakeGameplaySystem::is_game_failure(Global::reg))
            Global::gameplayUpdateSig(Global::reg); // effectively pauses game if failed or succeeded
        appstateCasted->previousTick += Global::DESIRED_TICK_PERIOD_MS;

        static auto previousMap = SnakeGameplaySystem::get_map(Global::reg);
        auto currentMap = SnakeGameplaySystem::get_map(Global::reg);
        if (currentMap != previousMap)
        {
            previousMap = currentMap;
            render_gameplay_visuals(Global::reg, appstateCasted->window, appstateCasted->renderer, Global::MAP_MARGIN_PX, Global::MAP_MARGIN_PX);
        }
    }
    SDL_Delay(Global::DESIRED_TICK_PERIOD_MS / 2U); // MUST BE DIVIDED BY >= 2U; saves some CPU

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
        case SDL_SCANCODE_UP:
            SnakeGameplaySystem::Control::up_key_down(Global::reg);
            break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
            SnakeGameplaySystem::Control::left_key_down(Global::reg);
            break;
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN:
            SnakeGameplaySystem::Control::down_key_down(Global::reg);
            break;
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
            SnakeGameplaySystem::Control::right_key_down(Global::reg);
            break;
        case SDL_SCANCODE_SPACE:
            SnakeGameplaySystem::Control::shift_key_down(Global::reg);
            break;
        case SDL_SCANCODE_R:
            if (SnakeGameplaySystem::is_game_failure(Global::reg) || SnakeGameplaySystem::is_game_success(Global::reg))
                init_gameplay_scene(Global::reg);
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
