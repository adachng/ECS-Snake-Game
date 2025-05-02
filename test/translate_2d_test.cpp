#include <gtest/gtest.h>

#include <component/position.hpp>
#include <component/velocity.hpp>
#include <component/delta_time.hpp>
#include <system/translate_2d.hpp>

namespace
{
    TEST(Translate2DSystemTest, ZeroVelocity)
    {
        entt::registry reg;
        auto entity = reg.create();
        reg.emplace<Position>(entity, 0.0f, 0.0f);
        reg.emplace<Velocity>(entity, 0.0f, 0.0f);
        reg.emplace<DeltaTime>(reg.create(), 100U);

        SystemTranslate2D::iterate(reg);

        Position pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, 0.0f);
        EXPECT_FLOAT_EQ(pos.y, 0.0f);

        for (int i = 1; i <= 100; i++)
            SystemTranslate2D::iterate(reg);

        pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, 0.0f);
        EXPECT_FLOAT_EQ(pos.y, 0.0f);
    }

    TEST(Translate2DSystemTest, PositiveVelocity)
    {
        entt::registry reg;
        auto entity = reg.create();
        reg.emplace<Position>(entity, 0.0f, 0.0f);
        reg.emplace<Velocity>(entity, 1.23f, 2.34f);
        reg.emplace<DeltaTime>(reg.create(), 100U);

        SystemTranslate2D::iterate(reg);

        Position pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, 0.0f + 0.123f);
        EXPECT_FLOAT_EQ(pos.y, 0.0f + 0.234f);

        float expectedX, expectedY;
        expectedX = expectedY = 0.0f;
        for (int i = 1; i <= 100; i++)
        {
            SystemTranslate2D::iterate(reg);
            expectedX += 0.123f;
            expectedY += 0.234;
        }
        expectedX += 0.123f;
        expectedY += 0.234f;

        pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, expectedX);
        EXPECT_FLOAT_EQ(pos.y, expectedY);
    }

    TEST(Translate2DSystemTest, NegativeVelocity)
    {
        entt::registry reg;
        auto entity = reg.create();
        reg.emplace<Position>(entity, 0.0f, 0.0f);
        reg.emplace<Velocity>(entity, -1.23f, -2.34f);
        reg.emplace<DeltaTime>(reg.create(), 100U);

        SystemTranslate2D::iterate(reg);

        Position pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, 0.0f - 0.123f);
        EXPECT_FLOAT_EQ(pos.y, 0.0f - 0.234f);

        float expectedX, expectedY;
        expectedX = expectedY = 0.0f;
        for (int i = 1; i <= 100; i++)
        {
            SystemTranslate2D::iterate(reg);
            expectedX -= 0.123f;
            expectedY -= 0.234f;
        }
        expectedX -= 0.123f;
        expectedY -= 0.234f;

        pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, expectedX);
        EXPECT_FLOAT_EQ(pos.y, expectedY);
    }

    TEST(Translate2DSystemTest, DeltaTimeMissing)
    {
        entt::registry reg;
        auto entity = reg.create();
        reg.emplace<Position>(entity, 0.0f, 0.0f);
        reg.emplace<Velocity>(entity, 12.0f, -100.0f);

        for (int i = 1; i <= 100; i++)
            SystemTranslate2D::iterate(reg);

        Position pos = reg.get<Position>(reg.view<Position>().front());

        EXPECT_FLOAT_EQ(pos.x, 0.0f);
        EXPECT_FLOAT_EQ(pos.y, 0.0f);
    }

    TEST(Translate2DSystemTest, MultipleInit)
    {
        sigslot::signal<entt::registry &> gameplaySceneSignal;
        sigslot::signal<entt::registry &> mainMenuSceneSignal;
        sigslot::signal<entt::registry &> creditsSceneSignal;

        EXPECT_TRUE(SystemTranslate2D::init(gameplaySceneSignal));
        EXPECT_TRUE(SystemTranslate2D::init(mainMenuSceneSignal));
        EXPECT_TRUE(SystemTranslate2D::init(creditsSceneSignal));

        EXPECT_FALSE(SystemTranslate2D::init(gameplaySceneSignal));
        EXPECT_FALSE(SystemTranslate2D::init(mainMenuSceneSignal));
        EXPECT_FALSE(SystemTranslate2D::init(creditsSceneSignal));

        EXPECT_FALSE(SystemTranslate2D::init(gameplaySceneSignal));
        EXPECT_FALSE(SystemTranslate2D::init(mainMenuSceneSignal));
        EXPECT_FALSE(SystemTranslate2D::init(creditsSceneSignal));
    }
} // namespace
