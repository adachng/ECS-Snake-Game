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
    // TEST(SnakeGameplaySystemTest, AppleSpawn)
    // {
    //     // TODO: WIP
    // }

    TEST(SnakeGameplaySystemTest, TrailingWithoutApple)
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

        SnakeGameplaySystem::update(registry); // to set the velocity of the snake head based on 'd'
        SystemTranslate2D::update(registry);   // 0.1s has passed
        SnakeGameplaySystem::update(registry);

        using namespace SnakeGameplaySystem;
        std::vector<std::vector<MapSlotState>> comp(1, std::vector<MapSlotState>(3, MapSlotState::EMPTY));
        comp[0][2] = MapSlotState::SNAKE_HEAD;
        comp[0][1] = MapSlotState::SNAKE_BODY;

        SnakeGameplaySystem::Debug::print_map(SnakeGameplaySystem::get_map(registry)); // NOTE: toggle to see
        SDL_Log("comp =");                                                             // NOTE: toggle to see
        SnakeGameplaySystem::Debug::print_map(comp);                                   // NOTE: toggle to see
        EXPECT_TRUE(SnakeGameplaySystem::get_map(registry) == comp);
    }

    // TEST(SnakeGameplaySystemTest, TrailingWithApple)
    // {
    //     entt::registry registry;
    //     { // create game state entity; 3x1 map
    //         auto entity = registry.create();
    //         registry.emplace<KeyControl>(entity, 'd');
    //         registry.emplace<DeltaTime>(entity, 100U);
    //         registry.emplace<SnakeBoundary2D>(entity, 3, 1);
    //     }
    //     { // create apple
    //         auto entity = registry.create();
    //         registry.emplace<Position>(entity, 1.5f, 0.5f);
    //         registry.emplace<SnakeApple>(entity);
    //     }
    //     { // create snake head
    //         auto entity = registry.create();
    //         registry.emplace<Position>(entity, 0.5f, 0.5f);
    //         registry.emplace<Velocity>(entity, 0.0f, 0.0f);
    //         registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
    //     }
    // }

    // TEST(SnakeGameplaySystemTest, HeadOnlyEatApple)
    // {
    // entt::registry registry;
    // { // create game state entity; 3x1 map
    //     auto entity = registry.create();
    //     registry.emplace<KeyControl>(entity, 'd');
    //     registry.emplace<DeltaTime>(entity, 100U);
    //     registry.emplace<SnakeBoundary2D>(entity, 3, 1);
    // }
    // { // create apple
    //     auto entity = registry.create();
    //     registry.emplace<Position>(entity, 1.5f, 0.5f);
    //     registry.emplace<SnakeApple>(entity);
    // }
    // { // create snake head
    //     auto entity = registry.create();
    //     registry.emplace<Position>(entity, 0.5f, 0.5f);
    //     registry.emplace<Velocity>(entity, 0.0f, 0.0f);
    //     registry.emplace<SnakePartHead>(entity, 10.0f, 1.0f); // 10 /s speed
    // }
    // }
} // namespace
