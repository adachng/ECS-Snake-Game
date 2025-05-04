#ifndef SRC_SYSTEM_SNAKE_DISCRETE_VECTOR_HPP
#define SRC_SYSTEM_SNAKE_DISCRETE_VECTOR_HPP

#include <vector>

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <entt/entt.hpp>
#include <sigslot/signal.hpp>

#include <component/velocity.hpp>
#include <component/key_control.hpp>
#include <component/snake_apple.hpp>
#include <component/snake_part.hpp>
#include <component/snake_part_head.hpp>
#include <component/snake_boundary_2d.hpp>

namespace SnakeGameplaySystem
{
    enum MapSlotState : Uint8
    {
        EMPTY = 0b0000U,
        SNAKE_HEAD = 0b0001U,
        SNAKE_BODY = 0b0010U,
        APPLE = 0b0100U,

        ENUM_END = 0b1111U,
    }; // enum MapSlotState

    namespace Util
    {
        static void to_grid_cell(const Position &pos, long *x, long *y)
        {
            // value >= 0.0f and value < 1.0f is inside of 1
            // if < 0.0f, it is 0; if >= 1.0f, it is 2
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
        static void to_indices(const Position &pos, long *x, long *y, const long &sizeY)
        {
            to_grid_cell(pos, x, y);
            *x = *x - 1;
            *y = sizeY - *y;
            if (sizeY == 1) // solves segfault
                *y = 0;
        }

        static Position from_grid_cell(const long &x, const long &y)
        {
            Position ret;
            ret.x = static_cast<float>(x) - 0.5f;
            ret.y = static_cast<float>(y) - 0.5f;
            return ret;
        }
        static Position from_indices(const long &x, const long &y, const long &sizeY)
        {
            return from_grid_cell(x + 1, sizeY - y);
        }
    } // namespace Util

    template <typename T = entt::entity>
    static T *get_snake_body_at_indices(entt::registry &reg, const int &targetI, const int &targetJ, const int &sizeY);

    static std::vector<std::vector<MapSlotState>> get_map(entt::registry &reg);
    static bool is_game_success(entt::registry &reg);
    static bool is_game_failure(entt::registry &reg);
    static bool is_going_backwards(entt::registry &reg, const char &directionToGo);
    static void do_trailing(entt::registry &reg, const bool &isAteApple)
    { // NOTE: this function is the reason why the update loop NEEDS to limit DeltaTime
        auto currentMap = get_map(reg);
        static auto previousMap = get_map(reg); // TODO: static is not good for different source file usage
        if (currentMap == previousMap)
            return;

        struct Index
        {
            explicit Index(const int &_i, const int &_j) : i(_i), j(_j) {}
            int i;
            int j;
        }; // struct Index

        auto getSnakeHeadIndices = [](const std::vector<std::vector<MapSlotState>> &map)
        {
            Index ret(-1, -1);
            for (int i = 0; i < map.size(); i++)
            {
                for (int j = 0; j < map[i].size(); j++)
                {
                    if (map[i][j] & MapSlotState::SNAKE_HEAD)
                        return Index(i, j);
                }
            }
            return ret;
        };

        Index previousSnakeHeadIndex = getSnakeHeadIndices(previousMap);
        Index currentSnakeHeadIndex = getSnakeHeadIndices(currentMap);

        if (previousSnakeHeadIndex.i == currentSnakeHeadIndex.i && previousSnakeHeadIndex.j == currentSnakeHeadIndex.j)
        {
            previousMap = currentMap;
            return;
        }

        char travelledDirection = '\t';
        if (currentSnakeHeadIndex.i < previousSnakeHeadIndex.i)
            travelledDirection = 'w';
        else if (currentSnakeHeadIndex.j < previousSnakeHeadIndex.j)
            travelledDirection = 'a';
        else if (currentSnakeHeadIndex.i > previousSnakeHeadIndex.i)
            travelledDirection = 's';
        else if (currentSnakeHeadIndex.j > previousSnakeHeadIndex.j)
            travelledDirection = 'd';

        previousMap = currentMap;
        SDL_assert(travelledDirection != '\t');
        if (travelledDirection == '\t')
            return;

        auto snakePartView = reg.view<SnakePart>();
        if (snakePartView.empty())
        {
            if (!isAteApple)
                return;

            auto snakeBoundaryView = reg.view<SnakeBoundary2D>();
            SDL_assert(snakeBoundaryView.size() == 1);
            const SnakeBoundary2D boundary = reg.get<SnakeBoundary2D>(snakeBoundaryView.front());
            const int i = currentSnakeHeadIndex.i, j = currentSnakeHeadIndex.j;
            switch (travelledDirection)
            {
            case 'w':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 'w');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(j, i + 1, boundary.y));
                break;
            }
            case 'a':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 'a');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(j + 1, i, boundary.y));
                break;
            }
            case 's':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 's');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(j, i - 1, boundary.y));
                break;
            }
            case 'd':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 'd');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(j - 1, i, boundary.y));
                break;
            }
            } // switch (travelledDirection)
        }
        else
        {
            auto snakeBoundaryView = reg.view<SnakeBoundary2D>();
            SDL_assert(snakeBoundaryView.size() == 1);
            const SnakeBoundary2D boundary = reg.get<SnakeBoundary2D>(snakeBoundaryView.front());
            int i = currentSnakeHeadIndex.i, j = currentSnakeHeadIndex.j;
            switch (travelledDirection)
            {
            case 'w':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 'w');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(j, ++i, boundary.y));
                break;
            }
            case 'a':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 'a');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(++j, i, boundary.y));
                break;
            }
            case 's':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 's');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(j, --i, boundary.y));
                break;
            }
            case 'd':
            {
                auto entitySnakePart = reg.create();
                reg.emplace<SnakePart>(entitySnakePart, 'd');
                reg.emplace<Position>(entitySnakePart, Util::from_indices(--j, i, boundary.y));
                break;
            }
            } // switch (travelledDirection)

            if (!isAteApple)
            {
                // At this point, i and j are at the neck.
                // So we just go to the opposite direction until
                // we either hit a wall or empty space. When that happens,
                // that is the body part we need to destroy.
                auto traverseOppositeOfDirection = [](const char &c, int &i, int &j)
                {
                    switch (c)
                    {
                    case 'w':
                        i++;
                        break;
                    case 'a':
                        j++;
                        break;
                    case 's':
                        i--;
                        break;
                    case 'd':
                        j--;
                        break;
                    default:
                        break;
                    }
                    return;
                };
                int previousI = i, previousJ = j;
                auto *entity_ptr = get_snake_body_at_indices(reg, i, j, boundary.y);
                while (entity_ptr != nullptr)
                {
                    previousI = i;
                    previousJ = j;
                    traverseOppositeOfDirection(reg.get<SnakePart>(*entity_ptr).currentDirection, i, j);
                    entity_ptr = get_snake_body_at_indices(reg, i, j, boundary.y);
                }
                entity_ptr = get_snake_body_at_indices(reg, previousI, previousJ, boundary.y);
                reg.destroy(*entity_ptr);
            }
        }
    }
    static bool apple_update(entt::registry &reg)
    {
        auto map = get_map(reg);
        struct Index
        {
            explicit Index(const int &_i, const int &_j) : i(_i), j(_j) {}
            int i;
            int j;
        };
        std::vector<Index> indexVec;
        bool isEaten = false;
        for (int i = 0; i < map.size(); i++)
        {
            for (int j = 0; j < map[i].size(); j++)
            {
                if (map[i][j] == MapSlotState::EMPTY)
                    indexVec.push_back(Index(i, j));
                else if ((map[i][j] & MapSlotState::SNAKE_HEAD) && (map[i][j] & MapSlotState::APPLE))
                {
                    isEaten = true;
                }
            }
        }
        do_trailing(reg, isEaten);

        auto respawnApple = [](entt::registry &reg, const std::vector<Index> &indexVec)
        {
            auto appleView = reg.view<SnakeApple, Position>();
            SDL_assert(appleView.storage<SnakeApple>()->size() <= 1);
            const bool isNoApple = appleView.storage<SnakeApple>()->empty();
            if (!isNoApple) // don't spawn in when there's no apple
            {
                if (indexVec.empty())
                    reg.destroy(appleView.front());
                else
                {
                    Position &applePos = reg.get<Position>(appleView.front());
                    const Sint32 indexVexIndex = SDL_rand(indexVec.size());
                    const int y = indexVec[indexVexIndex].i;
                    const int x = indexVec[indexVexIndex].j;

                    auto snakeBoundaryView = reg.view<SnakeBoundary2D>();
                    SDL_assert(snakeBoundaryView.size() == 1);
                    const SnakeBoundary2D boundary = reg.get<SnakeBoundary2D>(snakeBoundaryView.front());
                    applePos = Util::from_indices(x, y, boundary.y);
                }
            }
        };

        if (isEaten)
        {
            respawnApple(reg, indexVec);
            map = get_map(reg);

            bool hasErased = false;
            for (auto it = indexVec.begin(); it != indexVec.end();) // search for conflicting spots since the head was detached
            {                                                       // solves apple at neck issue
                Index index = *it;
                if ((map[index.i][index.j] & MapSlotState::APPLE) && map[index.i][index.j] > MapSlotState::APPLE)
                {
                    hasErased = true;
                    it = indexVec.erase(it); // erase() returns iterator to next element
                }
                else
                    ++it;
            }
            if (hasErased)
                respawnApple(reg, indexVec);
        }

        return isEaten;
    }
    static void iterate(entt::registry &reg)
    {
        if (is_game_success(reg))
            return;
        if (is_game_failure(reg))
            return;

        const bool ateApple = apple_update(reg);

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
                if (is_going_backwards(reg, 'w'))
                    break;
                vel.x = 0.0f;
                vel.y = headPart.speed;
                if (keyControl.isShiftKeyDown)
                    vel.y *= headPart.speedUpFactor;
                break;
            case 'a':
                if (is_going_backwards(reg, 'a'))
                    break;
                vel.x = -1.0f * headPart.speed;
                if (keyControl.isShiftKeyDown)
                    vel.x *= headPart.speedUpFactor;
                vel.y = 0.0f;
                break;
            case 's':
                if (is_going_backwards(reg, 's'))
                    break;
                vel.x = 0.0f;
                vel.y = -1.0f * headPart.speed;
                if (keyControl.isShiftKeyDown)
                    vel.y *= headPart.speedUpFactor;
                break;
            case 'd':
                if (is_going_backwards(reg, 'd'))
                    break;
                vel.x = headPart.speed;
                if (keyControl.isShiftKeyDown)
                    vel.x *= headPart.speedUpFactor;
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

    template <typename T = entt::entity>
    static T *get_snake_body_at_indices(entt::registry &reg, const int &targetI, const int &targetJ, const int &sizeY)
    {
        T *ret = nullptr;

        auto snakeBoundaryView = reg.view<SnakeBoundary2D>();
        SDL_assert(snakeBoundaryView.size() == 1);
        const SnakeBoundary2D boundary = reg.get<SnakeBoundary2D>(snakeBoundaryView.front());

        auto view = reg.view<Position, SnakePart>();
        for (auto entity : view)
        {
            Position pos = reg.get<Position>(entity);
            long i = -1L, j = -1L;
            SnakeGameplaySystem::Util::to_indices(pos, &j, &i, boundary.y);
            if (i == targetI && j == targetJ)
            {
                ret = &entity;
                break;
            }
        }
        return ret;
    }

    static bool is_going_backwards(entt::registry &reg, const char &directionToGo)
    {
        struct Index
        {
            explicit Index(const int &_i, const int &_j) : i(_i), j(_j) {}
            int i;
            int j;
            bool isInvalid() { return i < 0 || j < 0; }
        };
        auto map = get_map(reg);
        Index indexOfSnakeHead(-1, -1);
        for (int i = 0; i < map.size(); i++)
        {
            for (int j = 0; j < map[i].size(); j++)
            {
                if (map[i][j] == MapSlotState::SNAKE_HEAD)
                    indexOfSnakeHead = Index(i, j);
            }
        }
        if (indexOfSnakeHead.isInvalid())
            return true;

        int i = indexOfSnakeHead.i, j = indexOfSnakeHead.j;
        switch (directionToGo)
        { // check if it's a wall or not a snake body
        case 'w':
            if (i == 0)
                return false;
            if (map[i - 1][j] != MapSlotState::SNAKE_BODY)
                return false;
            break;
        case 'a':
            if (j == 0)
                return false;
            if (map[i][j - 1] != MapSlotState::SNAKE_BODY)
                return false;
            break;
        case 's':
            if (i == map.size() - 1)
                return false;
            if (map[i + 1][j] != MapSlotState::SNAKE_BODY)
                return false;
            break;
        case 'd':
            if (j == map[i].size() - 1)
                return false;
            if (map[i][j + 1] != MapSlotState::SNAKE_BODY)
                return false;
            break;
        default:
            return true;
        }

        // It's a snake body at the directionToGo. Find out if it's
        // the "neck". If it's not, return false.
        auto snakeBoundaryView = reg.view<SnakeBoundary2D>();
        SDL_assert(snakeBoundaryView.size() == 1);
        const SnakeBoundary2D boundary = reg.get<SnakeBoundary2D>(snakeBoundaryView.front());

        auto view = reg.view<Position, SnakePart>();
        for (auto &entity : view)
        {
            switch (directionToGo)
            {
            // Get the body part in that direction and get its currentDirection.
            // If the currentDirection is opposite of directionToGo, return true.
            case 'w':
            {
                auto *entity_ptr = get_snake_body_at_indices(reg, i - 1, j, boundary.y);
                if (entity_ptr == nullptr)
                    return false;
                if (reg.get<SnakePart>(*entity_ptr).currentDirection == 's')
                    return true;
                break;
            }
            case 'a':
            {
                auto *entity_ptr = get_snake_body_at_indices(reg, i, j - 1, boundary.y);
                if (entity_ptr == nullptr)
                    return false;
                if (reg.get<SnakePart>(*entity_ptr).currentDirection == 'd')
                    return true;
                break;
            }
            case 's':
            {
                auto *entity_ptr = get_snake_body_at_indices(reg, i + 1, j, boundary.y);
                if (entity_ptr == nullptr)
                    return false;
                if (reg.get<SnakePart>(*entity_ptr).currentDirection == 'w')
                    return true;
                break;
            }
            case 'd':
            {
                auto *entity_ptr = get_snake_body_at_indices(reg, i, j + 1, boundary.y);
                if (entity_ptr == nullptr)
                    return false;
                if (reg.get<SnakePart>(*entity_ptr).currentDirection == 'a')
                    return true;
                break;
            }
            default:
                return true;
            }
        }
        return false;
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
            SnakeGameplaySystem::Util::to_indices(pos, &xIndex, &yIndex, boundary.y);
            if (xIndex >= 0 && yIndex >= 0 && yIndex < ret.size() && xIndex < ret[yIndex].size())
                ret[yIndex][xIndex] = MapSlotState::SNAKE_BODY;
        }

        auto snakePartHeadView = reg.view<SnakePartHead, Position>();
        for (auto entity : snakePartHeadView)
        {
            const Position &pos = snakePartView.get<Position>(entity);
            long xIndex, yIndex;
            SnakeGameplaySystem::Util::to_indices(pos, &xIndex, &yIndex, boundary.y);
            if (xIndex >= 0 && yIndex >= 0 && yIndex < ret.size() && xIndex < ret[yIndex].size())
            {
                Uint8 entry = static_cast<Uint8>(ret[yIndex][xIndex]) | MapSlotState::SNAKE_HEAD;
                ret[yIndex][xIndex] = static_cast<MapSlotState>(entry);
            }
        }

        auto appleView = reg.view<SnakeApple, Position>();
        for (auto entity : appleView)
        {
            const Position &pos = appleView.get<Position>(entity);
            long xIndex, yIndex;
            SnakeGameplaySystem::Util::to_indices(pos, &xIndex, &yIndex, boundary.y);
            if (xIndex >= 0 && yIndex >= 0 && yIndex < ret.size() && xIndex < ret[yIndex].size())
            {
                Uint8 entry = static_cast<Uint8>(ret[yIndex][xIndex]) | MapSlotState::APPLE;
                ret[yIndex][xIndex] = static_cast<MapSlotState>(entry);
            }
        }

        return ret;
    }

    static bool is_game_success(entt::registry &reg)
    {
        auto map = get_map(reg);
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

    static bool is_game_failure(entt::registry &reg)
    {
        auto snakeHeadPos = reg.get<Position>(reg.view<SnakePartHead, Position>().front());
        auto boundary = reg.get<SnakeBoundary2D>(reg.view<SnakeBoundary2D>().front());
        if (snakeHeadPos.x < 0.0f || snakeHeadPos.x >= boundary.x || snakeHeadPos.y < 0.0f || snakeHeadPos.y >= boundary.y)
            return true;

        auto map = get_map(reg);
        for (int i = 0; i < map.size(); i++)
        {
            for (int j = 0; j < map[i].size(); j++)
            {
                if ((static_cast<Uint8>(map[i][j]) & SNAKE_HEAD) && (static_cast<Uint8>(map[i][j]) & SNAKE_BODY))
                    return true;
            }
        }
        return false;
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
                    if (static_cast<Uint8>(map[i][j]) == EMPTY)
                        str += ".";
                    if (static_cast<Uint8>(map[i][j]) & SNAKE_HEAD)
                        str += "$";
                    if (static_cast<Uint8>(map[i][j]) & SNAKE_BODY)
                        str += "x";
                    if (static_cast<Uint8>(map[i][j]) & APPLE)
                        str += "@";
                    str += " "; // allows manual checking for collisions
                }
                if (i < map.size() - 1)
                    str += "\n\t";
            }
            SDL_Log("\t%s", str.c_str());
        }
        static void print_snake_head_pos(const entt::registry &reg)
        {
            Position pos = get_snake_head_pos(reg);
            SDL_Log("\tSnakeHead is at Position(%f, %f)", pos.x, pos.y);
        }
        static void print_snake_head_vel(const entt::registry &reg)
        {
            Velocity vel = get_snake_head_velocity(reg);
            SDL_Log("\tSnakeHead is at Velocity(%f, %f)", vel.x, vel.y);
        }
    } // namespace Debug
} // namespace SnakeDiscreteVector

#endif // SRC_SYSTEM_SNAKE_DISCRETE_VECTOR_HPP
