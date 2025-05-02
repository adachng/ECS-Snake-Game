#ifndef SRC_SYSTEM_SNAKE_DISCRETE_VECTOR_HPP
#define SRC_SYSTEM_SNAKE_DISCRETE_VECTOR_HPP

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_log.h>
#include <entt/entt.hpp>
#include <sigslot/signal.hpp>

#include <component/velocity.hpp>
#include <component/key_control.hpp>
#include <component/snake_part.hpp>
#include <component/snake_part_head.hpp>
#include <component/snake_boundary_2d.hpp>

namespace SnakeGameplaySystem
{
    enum MapSlotState
    {
        EMPTY = 0b000,
        SNAKE_HEAD = 0b001,
        SNAKE_BODY = 0b010,
        APPLE = 0b100,
        ENUM_END = 0b111,
    }; // enum MapSlotState

    namespace Util
    {
        static void to_grid_cell(const Position &pos, long *x, long *y)
        {
            if (x != nullptr)
            {
                *x = SDL_lroundf(SDL_ceilf(pos.x));
                if (pos.x == *x)
                    *x += 1L;
            }
            if (y != nullptr)
            {
                *y = SDL_lroundf(SDL_ceilf(pos.y));
                if (pos.y == *y)
                    *y += 1L;
            }
        }
    } // namespace Util

    static bool is_game_success(entt::registry &reg);
    static void iterate(entt::registry &reg)
    {
        if (is_game_success(reg))
            return;
        // TODO: WIP
        auto keyControlView = reg.view<KeyControl>();
        SDL_assert(keyControlView.size() == 1);
        KeyControl keyControl = reg.get<KeyControl>(keyControlView.front());

        auto snakeHeadView = reg.view<Velocity, SnakePartHead>();
        SDL_assert(snakeHeadView.storage<SnakePartHead>()->size() == 1);
        for (auto entity : snakeHeadView)
        {
            Velocity &vel = snakeHeadView.get<Velocity>(entity);
            SnakePartHead &headPart = snakeHeadView.get<SnakePartHead>(entity);
            switch (keyControl.lastMovementKeyDown)
            {
            case 'w':
                vel.x = 0.0f;
                vel.y = headPart.speed;
                break;
            case 'a':
                vel.x = -1.0f * headPart.speed;
                vel.y = 0.0f;
                break;
            case 's':
                vel.x = 0.0f;
                vel.y = -1.0f * headPart.speed;
                break;
            case 'd':
                vel.x = headPart.speed;
                vel.y = 0.0f;
                break;
            default:
                break;
            }
        }
    }
    static void update(entt::registry &reg) { return iterate(reg); }

    static bool init(sigslot::signal<entt::registry &> &signal)
    {
        static std::list<sigslot::signal<entt::registry &> *> regSignalArray;
        bool ret = true;
        for (auto connectedSignal : regSignalArray)
        {
            if (connectedSignal == &signal)
            {
                ret = false;
                break;
            }
        }
        if (ret)
        {
            signal.connect(SnakeGameplaySystem::iterate);
            regSignalArray.push_back(&signal);
        }
        return ret;
    }

    static std::vector<std::vector<MapSlotState>> get_map(entt::registry &reg)
    {
        auto snakeBoundaryView = reg.view<SnakeBoundary2D>();
        SDL_assert(snakeBoundaryView.size() == 1);
        const SnakeBoundary2D boundary = reg.get<SnakeBoundary2D>(snakeBoundaryView.front());

        const int xSize = boundary.x;
        const int ySize = boundary.y;

        std::vector<std::vector<MapSlotState>> ret(ySize, std::vector<MapSlotState>(xSize, MapSlotState::EMPTY));

        auto snakePartView = reg.view<SnakePart, Position>();
        for (auto entity : snakePartView)
        {
            const Position &pos = snakePartView.get<Position>(entity);
            long xIndex, yIndex;
            SnakeGameplaySystem::Util::to_grid_cell(pos, &xIndex, &yIndex);
            yIndex = boundary.y - yIndex;
            ret[yIndex][xIndex - 1] = MapSlotState::SNAKE_BODY;
        }

        auto snakePartHeadView = reg.view<SnakePartHead, Position>();
        for (auto entity : snakePartHeadView)
        {
            const Position &pos = snakePartView.get<Position>(entity);
            long xIndex, yIndex;
            SnakeGameplaySystem::Util::to_grid_cell(pos, &xIndex, &yIndex);
            yIndex = boundary.y - yIndex;
            ret[yIndex][xIndex - 1] = MapSlotState::SNAKE_HEAD;
        }

        return ret;
    }

    static bool is_game_success(entt::registry &reg)
    {
        std::vector<std::vector<MapSlotState>> map = get_map(reg);
        for (int i = 0; i < map.size(); i++)
        {
            for (int j = 0; j < map[i].size(); j++)
            {
                if (map[i][j] == MapSlotState::EMPTY || map[i][j] == MapSlotState::APPLE)
                    return false;
            }
        }
        return true;
    }

    namespace Control
    {
        static void shift_key_up(entt::registry &reg)
        {
            auto keyControlView = reg.view<KeyControl>();
            SDL_assert(keyControlView.size() == 1);
            keyControlView.each([](KeyControl &control)
                                { control.isShiftKeyDown = false; });
        }
        static void shift_key_down(entt::registry &reg)
        {
            auto keyControlView = reg.view<KeyControl>();
            SDL_assert(keyControlView.size() == 1);
            keyControlView.each([](KeyControl &control)
                                { control.isShiftKeyDown = true; });
        }
        static void up_key_down(entt::registry &reg)
        {
            auto keyControlView = reg.view<KeyControl>();
            SDL_assert(keyControlView.size() == 1);
            keyControlView.each([](KeyControl &control)
                                { control.lastMovementKeyDown = 'w'; });
        }
        static void left_key_down(entt::registry &reg)
        {
            auto keyControlView = reg.view<KeyControl>();
            SDL_assert(keyControlView.size() == 1);
            keyControlView.each([](KeyControl &control)
                                { control.lastMovementKeyDown = 'a'; });
        }
        static void down_key_down(entt::registry &reg)
        {
            auto keyControlView = reg.view<KeyControl>();
            SDL_assert(keyControlView.size() == 1);
            keyControlView.each([](KeyControl &control)
                                { control.lastMovementKeyDown = 's'; });
        }
        static void right_key_down(entt::registry &reg)
        {
            auto keyControlView = reg.view<KeyControl>();
            SDL_assert(keyControlView.size() == 1);
            keyControlView.each([](KeyControl &control)
                                { control.lastMovementKeyDown = 'd'; });
        }
    } // namespace Control

    namespace Debug
    {
        static Position get_snake_head_pos(const entt::registry &reg)
        {
            auto view = reg.view<Position, SnakePartHead>();
            SDL_assert(view.storage<SnakePartHead>()->size() == 1);
            return reg.get<Position>(view.front());
        }

        static Velocity get_snake_head_velocity(const entt::registry &reg)
        {
            auto view = reg.view<Velocity, SnakePartHead>();
            SDL_assert(view.storage<SnakePartHead>()->size() == 1);
            return reg.get<Velocity>(view.front());
        }

        static void print_map(const std::vector<std::vector<MapSlotState>> &map)
        {
            std::string str = "";
            for (int i = 0; i < map.size(); i++)
            {
                for (int j = 0; j < map[i].size(); j++)
                {
                    switch (map[i][j])
                    {
                    case SNAKE_HEAD:
                        str += "$ ";
                        break;
                    case SNAKE_BODY:
                        str += "x ";
                        break;
                    case APPLE:
                        str += "@ ";
                        break;
                    default:
                        str += ". ";
                        break;
                    }
                }
                str += "\n";
            }
            SDL_Log(str.c_str());
        }
    } // namespace Debug
} // namespace SnakeDiscreteVector

#endif // SRC_SYSTEM_SNAKE_DISCRETE_VECTOR_HPP
