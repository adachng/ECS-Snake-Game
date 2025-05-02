#include <gtest/gtest.h>

#include <component/position.hpp>
#include <component/delta_time.hpp>
#include <component/snake_part.hpp>
#include <component/snake_part_head.hpp>
#include <component/snake_boundary_2d.hpp>
#include <component/snake_apple.hpp>
#include <system/snake_gameplay_system.hpp>

namespace
{
    TEST(SDLMathTest, Ceil)
    {
        for (int i = 0; i <= 100; i++)
            EXPECT_EQ(SDL_lroundf(SDL_ceilf(static_cast<float>(i))), i);
    }

    TEST(SnakeGameplaySystemTest, ValidChangeDirection)
    {
        entt::registry registry;
        auto entity = registry.create();
        registry.emplace<KeyControl>(entity);
        registry.emplace<DeltaTime>(entity, 5000U);
        registry.emplace<SnakeBoundary2D>(entity, 20, 20);

        auto entitySnakeHead = registry.create();
        registry.emplace<Position>(entitySnakeHead, 10.0f, 10.0f);
        registry.emplace<Velocity>(entitySnakeHead, 0.0f, 0.0f);
        registry.emplace<SnakePartHead>(entitySnakeHead, 1.23f, 1.5f);

        SnakeGameplaySystem::Control::up_key_down(registry);
        SnakeGameplaySystem::update(registry);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, 0.0f);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, 1.23f);

        SnakeGameplaySystem::Control::left_key_down(registry);
        SnakeGameplaySystem::update(registry);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, -1.23f);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, 0.0f);

        SnakeGameplaySystem::Control::down_key_down(registry);
        SnakeGameplaySystem::update(registry);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, 0.0f);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, -1.23f);

        SnakeGameplaySystem::Control::right_key_down(registry);
        SnakeGameplaySystem::update(registry);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, 1.23f);
        EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, 0.0f);
    }

    TEST(SnakeGameplaySystemTest, InvalidChangeDirection)
    {
        // TODO: WIP
        // entt::registry registry;
        // auto entity = registry.create();
        // registry.emplace<KeyControl>(entity);
        // registry.emplace<DeltaTime>(entity, 5000U);
        // registry.emplace<SnakeBoundary2D>(entity, 20, 20);

        // auto entitySnakeHead = registry.create();
        // registry.emplace<Position>(entitySnakeHead, 10.0f, 10.0f);
        // registry.emplace<Velocity>(entitySnakeHead, 0.0f, 0.0f);
        // registry.emplace<SnakePartHead>(entitySnakeHead, 1.0f, 1.5f);

        // SnakeGameplaySystem::Control::up_key_down(registry);
        // SnakeGameplaySystem::update(registry);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, 0.0f);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, 1.0f);

        // SnakeGameplaySystem::Control::left_key_down(registry);
        // SnakeGameplaySystem::update(registry);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, -1.0f);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, 0.0f);

        // SnakeGameplaySystem::Control::down_key_down(registry);
        // SnakeGameplaySystem::update(registry);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, 0.0f);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, -1.0f);

        // SnakeGameplaySystem::Control::right_key_down(registry);
        // SnakeGameplaySystem::update(registry);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).x, 1.0f);
        // EXPECT_FLOAT_EQ(SnakeGameplaySystem::Debug::get_snake_head_velocity(registry).y, 0.0f);
    }

    TEST(SnakeGameplaySystemTest, ShiftKeySpeedUp)
    {
        // TODO: WIP
    }

    TEST(SnakeGameplaySystemTest, DiscretisePosition)
    {
        Position pos;
        long gridX, gridY;

        pos.y = pos.x = 0.0f;
        SnakeGameplaySystem::Util::to_grid_cell(pos, &gridX, &gridY);
        EXPECT_EQ(gridX, 1L);
        EXPECT_EQ(gridY, 1L);

        pos.y = pos.x = 0.1f;
        SnakeGameplaySystem::Util::to_grid_cell(pos, &gridX, &gridY);
        EXPECT_EQ(gridX, 1L);
        EXPECT_EQ(gridY, 1L);

        pos.y = pos.x = 1.0f;
        SnakeGameplaySystem::Util::to_grid_cell(pos, &gridX, &gridY);
        EXPECT_EQ(gridX, 2L);
        EXPECT_EQ(gridY, 2L);

        pos.y = pos.x = 4.99999f;
        SnakeGameplaySystem::Util::to_grid_cell(pos, &gridX, &gridY);
        EXPECT_EQ(gridX, 5L);
        EXPECT_EQ(gridY, 5L);

        pos.y = pos.x = 5.0f;
        SnakeGameplaySystem::Util::to_grid_cell(pos, &gridX, &gridY);
        EXPECT_EQ(gridX, 6L);
        EXPECT_EQ(gridY, 6L);
    }

    TEST(SnakeGameplaySystemTest, GetMap)
    {
        entt::registry registry;
        auto entity = registry.create();
        registry.emplace<KeyControl>(entity);
        registry.emplace<DeltaTime>(entity, 5000U);
        registry.emplace<SnakeBoundary2D>(entity, 6, 5);

        auto entitySnakeHead = registry.create();
        registry.emplace<Position>(entitySnakeHead, 0.0f, 0.0f);
        registry.emplace<Velocity>(entitySnakeHead, 0.0f, 0.0f);
        registry.emplace<SnakePartHead>(entitySnakeHead, 1.0f, 1.5f);

        auto makeBody = [&registry](const float &posX, const float &posY, const char &currentDirection)
        {
        auto entitySnakeBody = registry.create();
        registry.emplace<Position>(entitySnakeBody, posX, posY);
        registry.emplace<SnakePart>(entitySnakeBody, currentDirection); };

        // . . . . x x
        // . . . . x .
        // . x x x x .
        // . x . . . .
        // $ x . . . .
        makeBody(1.0f, 0.0f, 'a');
        makeBody(1.5f, 1.0f, 's');
        makeBody(1.9f, 2.5f, 's');
        makeBody(2.0f, 2.5f, 'a');
        makeBody(3.5f, 2.5f, 'a');
        makeBody(4.5f, 2.5f, 'a');
        makeBody(4.5f, 3.5f, 's');
        makeBody(4.5f, 4.5f, 's');
        makeBody(5.5f, 4.5f, 'a');

        // . . . . x x
        // . . . . x .
        // . x x x x .
        // . x . . . .
        // $ x . . . .
        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(5, std::vector<MapSlotState>(6, MapSlotState::EMPTY));
        comp[4][0] = MapSlotState::SNAKE_HEAD;
        comp[4][1] = MapSlotState::SNAKE_BODY;
        comp[3][1] = MapSlotState::SNAKE_BODY;
        comp[2][1] = MapSlotState::SNAKE_BODY;
        comp[2][2] = MapSlotState::SNAKE_BODY;
        comp[2][3] = MapSlotState::SNAKE_BODY;
        comp[2][4] = MapSlotState::SNAKE_BODY;
        comp[1][4] = MapSlotState::SNAKE_BODY;
        comp[0][4] = MapSlotState::SNAKE_BODY;
        comp[0][5] = MapSlotState::SNAKE_BODY;

        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);

        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
    }

    TEST(SnakeGameplaySystemTest, GameSuccess)
    {
        entt::registry registry1;
        {
            auto entity = registry1.create();
            registry1.emplace<KeyControl>(entity);
            registry1.emplace<DeltaTime>(entity, 5000U);
            registry1.emplace<SnakeBoundary2D>(entity, 1, 1);

            auto entitySnakeHead = registry1.create();
            registry1.emplace<Position>(entitySnakeHead, 0.5f, 0.5f);
            registry1.emplace<SnakePartHead>(entitySnakeHead, 0.0f, 0.0f);
        }

        EXPECT_TRUE(SnakeGameplaySystem::is_game_success(registry1));

        entt::registry registry2;
        {
            auto entity = registry2.create();
            registry2.emplace<KeyControl>(entity);
            registry2.emplace<DeltaTime>(entity, 5000U);
            registry2.emplace<SnakeBoundary2D>(entity, 2, 1);

            auto entitySnakeHead = registry2.create();
            registry2.emplace<Position>(entitySnakeHead, 0.5f, 0.5f);
            registry2.emplace<SnakePartHead>(entitySnakeHead, 0.0f, 0.0f);

            auto entitySnakeBody = registry2.create();
            registry2.emplace<Position>(entitySnakeBody, 1.5f, 0.5f);
            registry2.emplace<SnakePart>(entitySnakeBody);
        }
        EXPECT_TRUE(SnakeGameplaySystem::is_game_success(registry2));
    }

    TEST(SnakeGameplaySystemTest, GameFailure)
    {
    }
} // namespace
