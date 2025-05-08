#include <gtest/gtest.h>

#include <component/position.hpp>
#include <component/delta_time.hpp>
#include <component/snake_part.hpp>
#include <component/snake_part_head.hpp>
#include <component/snake_boundary_2d.hpp>
#include <component/snake_apple.hpp>
#include <system/translate_2d.hpp>
#include <system/snake_gameplay_system.hpp>

namespace
{
    TEST(SnakeGameplaySystemTest, TrailingOrthogonallyWithoutApple)
    {
        entt::registry registry;
        { // create game state entity; 3x1 map
            auto entity = registry.create();
            registry.emplace<KeyControl>(entity, 'd');
            registry.emplace<DeltaTime>(entity, 100U);
            registry.emplace<SnakeBoundary2D>(entity, 3, 1);
        }
        { // create snake head
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 0.5f);
            registry.emplace<Velocity>(entity, 0.0f, 0.0f);
            registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 0.5f);
            registry.emplace<SnakePart>(entity, 'd');
        }

        SnakeGameplaySystem::init(registry);
        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'd'
        SystemTranslate2D::update(registry);   // 0.1s has passed
        SnakeGameplaySystem::update(registry);

        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(1, std::vector<MapSlotState>(3, MapSlotState::EMPTY));
        comp[0][2] = MapSlotState::SNAKE_HEAD;
        comp[0][1] = MapSlotState::SNAKE_BODY;

        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SDL_Log("comp =");                                                             // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }

    TEST(SnakeGameplaySystemTest, TrailingDiagonallyWithoutApple)
    {
        entt::registry registry;
        { // create game state entity; 2x2 map
            auto entity = registry.create();
            registry.emplace<KeyControl>(entity, 'w');
            registry.emplace<DeltaTime>(entity, 100U);
            registry.emplace<SnakeBoundary2D>(entity, 2, 2);
        }
        { // create snake head
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 0.5f);
            registry.emplace<Velocity>(entity, 0.0f, 0.0f);
            registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 0.5f);
            registry.emplace<SnakePart>(entity, 'd');
        }

        // SDL_Log("turn 1 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see

        // SDL_Log("First update:");
        SnakeGameplaySystem::init(registry);
        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'd'
        SystemTranslate2D::update(registry);   // 0.1s has passed
        // SDL_Log("Second update:");
        SnakeGameplaySystem::update(registry);

        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(2, std::vector<MapSlotState>(2, MapSlotState::EMPTY));
        comp[0][1] = MapSlotState::SNAKE_HEAD;
        comp[1][1] = MapSlotState::SNAKE_BODY;

        // SDL_Log("turn 2 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SDL_Log("comp =");                                                             // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }

    TEST(SnakeGameplaySystemTest, TrailingOrthogonallyWithApple)
    {
        entt::registry registry;
        { // create game state entity; 4x1 map
            auto entity = registry.create();
            registry.emplace<KeyControl>(entity, 'd');
            registry.emplace<DeltaTime>(entity, 100U);
            registry.emplace<SnakeBoundary2D>(entity, 4, 1);
        }
        { // create apple
            auto entity = registry.create();
            registry.emplace<Position>(entity, 2.5f, 0.5f);
            registry.emplace<SnakeApple>(entity);
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 0.5f);
            registry.emplace<SnakePart>(entity, 'd');
        }
        { // create snake head
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 0.5f);
            registry.emplace<Velocity>(entity, 0.0f, 0.0f);
            registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
        }

        SnakeGameplaySystem::init(registry);
        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'd'
        // SDL_Log("turn 1 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_pos(registry);                    // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_vel(registry);                    // NOTE: toggle to see
        SystemTranslate2D::update(registry); // 0.1s has passed
        SnakeGameplaySystem::update(registry);

        // x x $ @
        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(1, std::vector<MapSlotState>(4, MapSlotState::EMPTY));
        comp[0][2] = MapSlotState::SNAKE_HEAD;
        comp[0][1] = MapSlotState::SNAKE_BODY;
        comp[0][0] = MapSlotState::SNAKE_BODY;
        comp[0][3] = MapSlotState::APPLE;

        // SDL_Log("turn 2 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_pos(registry);                    // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_vel(registry);                    // NOTE: toggle to see
        // SDL_Log("comp =");                                                             // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }

    TEST(SnakeGameplaySystemTest, TrailingDiagonallyWithApple)
    {
        entt::registry registry;
        { // create game state entity; 2x2 map
            auto entity = registry.create();
            registry.emplace<KeyControl>(entity, 'w');
            registry.emplace<DeltaTime>(entity, 100U);
            registry.emplace<SnakeBoundary2D>(entity, 2, 2);
        }
        { // create apple
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 1.5f);
            registry.emplace<SnakeApple>(entity);
        }
        { // create snake head
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 0.5f);
            registry.emplace<Velocity>(entity, 0.0f, 0.0f);
            registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 0.5f);
            registry.emplace<SnakePart>(entity, 'd');
        }

        // SDL_Log("turn 1 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see

        SnakeGameplaySystem::init(registry);
        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'd'
        SystemTranslate2D::update(registry);   // 0.1s has passed
        SnakeGameplaySystem::update(registry);

        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(2, std::vector<MapSlotState>(2, MapSlotState::EMPTY));
        comp[0][1] = MapSlotState::SNAKE_HEAD;
        comp[1][1] = MapSlotState::SNAKE_BODY;
        comp[1][0] = MapSlotState::SNAKE_BODY;
        comp[0][0] = MapSlotState::APPLE;

        // SDL_Log("turn 2 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SDL_Log("comp =");                                                             // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }

    TEST(SnakeGameplaySystemTest, HeadOnlyEatApple)
    {
        entt::registry registry;
        { // create game state entity; 3x1 map
            auto entity = registry.create();
            registry.emplace<KeyControl>(entity, 'd');
            registry.emplace<DeltaTime>(entity, 100U);
            registry.emplace<SnakeBoundary2D>(entity, 3, 1);
        }
        { // create apple
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 0.5f);
            registry.emplace<SnakeApple>(entity);
        }
        { // create snake head
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 0.5f);
            registry.emplace<Velocity>(entity, 0.0f, 0.0f);
            registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
        }

        SnakeGameplaySystem::init(registry);
        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'd'
        SystemTranslate2D::update(registry);   // 0.1s has passed
        SnakeGameplaySystem::update(registry); // apple eaten this iteration

        // x $ @
        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(1, std::vector<MapSlotState>(3, MapSlotState::EMPTY));
        comp[0][1] = MapSlotState::SNAKE_HEAD;
        comp[0][0] = MapSlotState::SNAKE_BODY;
        comp[0][2] = MapSlotState::APPLE;

        // SDL_Log("turn 1 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_pos(registry);                    // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_vel(registry);                    // NOTE: toggle to see
        // SDL_Log("comp =");                                                             // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }

    TEST(SnakeGameplaySystemTest, TailMoveOutOfWayBeforeHead)
    {
        entt::registry registry;
        { // create game state entity; 3x3 map
            auto entity = registry.create();
            registry.emplace<KeyControl>(entity, 'w');
            registry.emplace<DeltaTime>(entity, 100U);
            registry.emplace<SnakeBoundary2D>(entity, 3, 3);
        }
        { // create snake head
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 0.5f);
            registry.emplace<Velocity>(entity, 0.0f, 0.0f);
            registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.5f, 1.5f);
            registry.emplace<SnakePart>(entity, 'd');
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 1.5f);
            registry.emplace<SnakePart>(entity, 's');
        }
        { // create snake body
            auto entity = registry.create();
            registry.emplace<Position>(entity, 1.5f, 0.5f);
            registry.emplace<SnakePart>(entity, 'a');
        }
        // . . .
        // x x .
        // $ x . ; snake head is going upwards

        // SDL_Log("turn 1 ="); // NOTE: toggle to see
        SnakeGameplaySystem::init(registry);
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_pos(registry);                    // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_vel(registry);                    // NOTE: toggle to see

        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'w'
        SystemTranslate2D::update(registry);   // 0.1s has passed
        SnakeGameplaySystem::update(registry); // update system state

        // At this point, the game should neither fail nor succeed.
        EXPECT_FALSE(SnakeGameplaySystem::is_game_success(registry));
        EXPECT_FALSE(SnakeGameplaySystem::is_game_failure(registry));

        // x $ @
        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(3, std::vector<MapSlotState>(3, MapSlotState::EMPTY));
        comp[1][0] = MapSlotState::SNAKE_HEAD;
        comp[2][0] = MapSlotState::SNAKE_BODY;
        comp[2][1] = MapSlotState::SNAKE_BODY;
        comp[1][1] = MapSlotState::SNAKE_BODY;
        // . . .
        // $ x .
        // x x . ; snake head is going upwards

        // SDL_Log("turn 2 =");                                                           // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_pos(registry);                    // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_snake_head_vel(registry);                    // NOTE: toggle to see
        // SDL_Log("comp =");                                                             // NOTE: toggle to see
        // SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }
} // namespace
